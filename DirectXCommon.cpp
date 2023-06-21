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

}

/// <summary>
/// DXGIデバイスの初期化
/// </summary>
void DirectXCommon::InitializeDXGIDevice() {

	// 結果確認用
	HRESULT result = S_FALSE;

//#ifdef _DEBUG
//
//	ID3D12Debug1* debugController = nullptr;
//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
//		// デバックレイヤーを有効にする
//	}
//
//#endif // _DEBUG

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

}
