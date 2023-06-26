#include "DirectXCommon.h"

#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
/// <returns>シングルトンインスタンス</returns>
DirectXCommon* DirectXCommon::GetInstance() {
	// クラスのインスタンスを取得
	static DirectXCommon instance;
	// インスタンスを返す
	return &instance;
}

/// <summary>
/// 初期化処理
/// </summary>
/// <param name="win">ウィンドウズアプリケーションクラス</param>
/// <param name="backBufferWidth">ウィンドウ横幅</param>
/// <param name="backBufferHeight">ウィンドウ縦幅</param>
void DirectXCommon::Initialize(WinApp* win,
	int32_t backBufferWidth, int32_t backBufferHeight) {

	// 引数のNULLチェックを行う
	assert(win);

	// 引数の値をメンバ変数に代入
	winApp_ = win;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// DXGIデバイスの初期化
	InitializeDXGIDevice();
	// コマンド関連初期化
	InitializeCommand();
	// スワップチューン生成
	CreateSwapChain();
	// レンダーターゲット生成
	CreateFinalRenderTargets();
	// フェンス生成
	CreateFence();

}

/// <summary>
/// 描画前処理
/// </summary>
void DirectXCommon::PreDraw() {

	// バックバッファの番号を取得する
	// 二つの画面を用意してるので0~1。
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	// リソースバリアを変更する
	D3D12_RESOURCE_BARRIER barrier = SwitchResourceBarrier(
		backBuffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList_->ResourceBarrier(1, &barrier);

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(
		rtvHeap_.Get(), device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), bbIndex);

	// 画面をクリアする
	ClearRenderTarget();

}

/// <summary>
/// 描画終了後の処理
/// </summary>
void DirectXCommon::PostDraw() {

	// 結果確認用
	HRESULT result = S_FALSE;

	// リソースバリアを変更
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = SwitchResourceBarrier(
		backBuffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList_->ResourceBarrier(1, &barrier);

	// コマンドリストの命令をクローズする
	commandList_->Close();

	// GPUにコマンドリストの命令を実行するよう指示する
	ID3D12CommandList* commandLists[] = { commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行うように指示する
	swapChain_->Present(1, 0);

	// GPUの処理がここまでたどり着いた時にFenceに指定した値を代入するようSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);

	// FenceのSignal値が指定した値かどうかを確認する
	if (fence_->GetCompletedValue() < fenceVal_) {
		// FenceのSignalを待つためのイベントを生成
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		// イベントが生成出来ているか確認
		assert(fenceEvent != nullptr);
		// 指定したSignal値にたどり着いていないため、待機するイベントを設定
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent);
		// イベント終了まで待機
		WaitForSingleObject(fenceEvent, INFINITE);
		// 終了したら閉じる
		CloseHandle(fenceEvent);
	}

	// 次フレーム用にコマンドアロケーターをリセット
	result = commandAllocator_->Reset();
	// コマンドアロケーターが準備できたかを確認する
	assert(SUCCEEDED(result));
	// 次フレームようにコマンドリストをリセット
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	// コマンドリストをリセットできたか確認する
	assert(SUCCEEDED(result));

}

/// <summary>
/// レンダーターゲットのクリア
/// </summary>
void DirectXCommon::ClearRenderTarget() {

	// バックバッファのインデックスを取得する
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	// レンダーターゲットビューに用いるディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(
	rtvHeap_.Get(), device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), bbIndex);

	// 全画面クリア
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

}

/// <summary>
/// 特定のインデックスのディスクリプタハンドルを取得する関数(CPU)
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタサイズ</param>
/// <param name="index">取得するヒープのインデックス</param>
/// <returns>特定のインデックスのディスクリプタハンドル</returns>
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

/// <summary>
/// 特定のインデックスのディスクリプタハンドルを取得する関数(GPU)
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタサイズ</param>
/// <param name="index">取得するヒープのインデックス</param>
/// <returns>特定のインデックスのディスクリプタハンドル</returns>
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

/// <summary>
/// リソースバリアの切り替えを行う関数
/// </summary>
/// <param name="resource">リソース</param>
/// <param name="before">以前のステート</param>
/// <param name="after">以降のステート</param>
/// <returns>切り替えたリソースバリア</returns>
D3D12_RESOURCE_BARRIER DirectXCommon::SwitchResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {

	// 返り値用のリソースバリアを用意
	D3D12_RESOURCE_BARRIER barrier{};
	
	// 引数を元にバリアの設定を行う
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;

	// 設定を行ったバリアを返す
	return barrier;

}

/// <summary>
/// DXGIデバイスの初期化
/// </summary>
void DirectXCommon::InitializeDXGIDevice() {

	// 結果確認用
	HRESULT result = S_FALSE;

#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバックレイヤーを有効にする
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);

	}

#endif // _DEBUG

	// DXGIFactoryが生成できるかどうかを確認する
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	// 使用するアダプタを入れる変数　最初にnullptrを入れる
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;
	// いい順にアダプタを積む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {

		// アダプタの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = useAdapter->GetDesc3(&adapterDesc);
		// 取得できない場合はエラーを出す
		assert(SUCCEEDED(result));

		// 取得したアダプタがソフトウェアアダプタでなければ採用する
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			
			// 採用したアダプターの情報をログに入れて出力する
			Debug::Log(Debug::ConvertString(std::format(L"use Adapter:{}\n", adapterDesc.Description)));
			// 処理を抜ける
			break;
		}
		// ソフトウェアアダプタの場合は無視する
		useAdapter = nullptr;

	}

	// 適切なアダプタが見つからなかった場合は起動できない
	assert(useAdapter != nullptr);

	// 機能レベル
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	// ログ出力用文字列
	const char* featureLevelStriings[] = { "12.2", "12.1",  "12.0" };

	// 取得したアダプターを用いて、高い順にデバイスを生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタを使用してデバイスを生成
		result = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成出来ているかを確認する
		if (SUCCEEDED(result)) {
			// ログを出力
			Debug::Log(std::format("FeatureLevel : {}\n", featureLevelStriings[i]));
			// 生成できていればループ処理を抜ける
			break;
		}
	}

	// デバイスを生成出来なかった場合は起動しない
	assert(device_ != nullptr);
	// 初期化完了のログを出す
	Debug::Log("Complete Create D3D12Device!!!\n");

#ifdef _DEBUG

	// デバックでの実行時、エラーが出た場合警告を出す
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;

	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// 抑制するエラーID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制する表示レベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したエラーの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 重大なエラー時に停止する
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に停止する
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に停止する
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}

#endif // !_DEBUG


}

/// <summary>
/// スワップチューン作成関数
/// </summary>
void DirectXCommon::CreateSwapChain() {

	// 結果確認用
	HRESULT result = S_FALSE;

	// スワップチューンの生成を行う
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// スワップチューンの設定を行う
	swapChainDesc.Width = backBufferWidth_;						 // ウィンドウ横幅
	swapChainDesc.Height = backBufferHeight_;					 // ウィンドウ縦幅
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			 // 色情報の書式
	swapChainDesc.SampleDesc.Count = 1;							 // マルチサンプルを行わない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;								 // バッファ数を二つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // モニターの描画し次第破棄する

	// 変換のため、コームポインタで空のSwapChain1を生成する
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

	// コマンドキュー、ウィンドウハンドル、スワップチューンの設定を渡して生成する
	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc,
		nullptr, nullptr,
		&swapChain1
	);
	// スワップチューンを生成出来ているかを確認する
	assert(SUCCEEDED(result));

	// SwapChain1 を SwapChain4 に型変換して渡す
	result = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	// 渡せているかどうか確認する
	assert(SUCCEEDED(result));

}

/// <summary>
/// コマンド関連初期化関数
/// </summary>
void DirectXCommon::InitializeCommand() {

	// 結果確認用
	HRESULT result = S_FALSE;

	// コマンドキューの生成を行う
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	// 生成出来ているかを確認する
	assert(SUCCEEDED(result));

	// コマンドアロケーターの生成を行う
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// 生成出来ているか確認する
	assert(SUCCEEDED(result));

	// コマンドリストの生成を行う
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	// 生成出来ているか確認する
	assert(SUCCEEDED(result));
	

}

/// <summary>
/// レンダーターゲット生成
/// </summary>
void DirectXCommon::CreateFinalRenderTargets() {

	// 結果確認用
	HRESULT result = S_FALSE;

	// スワップチューンからdescを取得する
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapChain_->GetDesc(&swcDesc);
	// 取得出来ているか確認する
	assert(SUCCEEDED(result));

	// 各種設定を行い、ディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	heapDesc.NumDescriptors = swcDesc.BufferCount;
	result = device_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap_));
	// 生成出来ているかを確認する
	assert(SUCCEEDED(result));

	// 表と裏、二つの画面を生成する
	backBuffers_.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers_.size(); i++) {

		// スワップチューンからResourceを取得する
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		// 取得できているかを確認する
		assert(SUCCEEDED(result));

		// RTVの設定を行う
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 出力された結果をSRGBに変換し、書き込む
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2Dテクスチャ描画として書き込む

		// ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(
			rtvHeap_.Get(), device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), i);

		// レンダーターゲットを生成する
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);

	}

}

/// <summary>
/// フェンス生成関数
/// </summary>
void DirectXCommon::CreateFence() {

	// 結果確認用
	HRESULT result = S_FALSE;

	// フェンス生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	// フェンスを生成出来たか確認する
	assert(SUCCEEDED(result));

}