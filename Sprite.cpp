#include "Sprite.h"
#include <cassert>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dCompiler.lib")

using namespace Microsoft::WRL;

// 静的なメンバ変数の実体を宣言する

// デバイス
ID3D12Device* Sprite::sDevice_ = nullptr;

// コマンドリスト
ID3D12GraphicsCommandList* Sprite::sCommandList_ = nullptr;
// ルートシグネチャ
ComPtr<ID3D12RootSignature> Sprite::sRootSignature_;
// パイプラインステートオブジェクト
ComPtr<ID3D12PipelineState> Sprite::sPipelineStates_;

// dxcUtils
ComPtr<IDxcUtils> Sprite::dxcUtils_ = nullptr;
// dxcコンパイラ
ComPtr<IDxcCompiler3> Sprite::dxcCompiler_ = nullptr;
// InludeHandler
ComPtr<IDxcIncludeHandler> Sprite::dxcIncludeHandler_ = nullptr;

// 正射影行列
Matrix4x4 Sprite::sMatProjection_;

/// <summary>
/// 静的初期化関数
/// </summary>
/// <param name="device">DXGIデバイス</param>
/// <param name="window_Width">画面横幅</param>
/// <param name="window_Height">画面縦幅</param>
void Sprite::StaticInitialize(
	ID3D12Device* device
) {

	// 引数のNULLチェック
	assert(device);

	// 引数の値をメンバ変数に代入する
	sDevice_ = device;

	// 結果確認用
	HRESULT result = S_FALSE;

	// DXCの初期化
	InitializeDXC();

	// PSOの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	// ルートシグネチャを作成する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// ルートパラメータを設定する
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	// シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	result = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	// バイナリできているかを確認する
	if (FAILED(result)) {
		// エラーが出た場合ログにそれを出力
		Debug::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		// 停止させる
		assert(false);
	}

	// バイナリを元に実際にルートシグネチャを生成
	result = sDevice_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature_));
	// 生成出来ているかを確認する
	assert(SUCCEEDED(result));

	// インプットレイアウトの設定を行う
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescs;
	graphicsPipelineStateDesc.InputLayout.NumElements = _countof(inputElementDescs);

	// ブレンドステートの設定を行う
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// ラスタライザステートの設定を行う
	D3D12_RASTERIZER_DESC rasterrizerDesc{};
	// 背面カリング
	rasterrizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// 頂点シェーダ用のバイナリオブジェクト
	ComPtr<IDxcBlob> vertexBlob;
	// ピクセルシェーダ用のバイナリオブジェクト
	ComPtr<IDxcBlob> pixelBlob;

	// 頂点シェーダをコンパイルする
	vertexBlob = CompileShader(L"SpriteVS.hlsl", L"vs_6_0");
	// コンパイル出来たか確認する
	assert(vertexBlob != nullptr);
	// ピクセルシェーダをコンパイルする
	pixelBlob = CompileShader(L"SpritePS.hlsl", L"ps_6_0");
	// コンパイル出来たか確認する
	assert(pixelBlob != nullptr);

	// ルートシグネチャ
	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();
	// インプットレイアウト
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	// 頂点シェーダ
	graphicsPipelineStateDesc.VS = { vertexBlob->GetBufferPointer(),
	vertexBlob->GetBufferSize() };
	// ピクセルシェーダ
	graphicsPipelineStateDesc.PS = { pixelBlob->GetBufferPointer(),
	pixelBlob->GetBufferSize() };
	// ブレンドステート
	graphicsPipelineStateDesc.BlendState = blendDesc;
	// ラスタライザステート
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;
	
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジーのタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 3角形

	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 設定を元に実際に生成を行う
	result = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_));
	// 生成出来ているかを確認する
	assert(SUCCEEDED(result));

}

/// <summary>
/// 描画前処理
/// </summary>
/// <param name="cmdList">描画コマンドリスト</param>
void Sprite::PreDraw(ID3D12GraphicsCommandList* cmdList) {

	// PreDrawとPostDrawがペアで呼ばれていない場合はエラー
	assert(Sprite::sCommandList_ == nullptr);

	// コマンドリストをセットする
	sCommandList_ = cmdList;

	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::kWindowWidth;
	viewport.Height = WinApp::kwindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = WinApp::kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kwindowHeight;

	sCommandList_->RSSetViewports(1, &viewport);

	sCommandList_->RSSetScissorRects(1, &scissorRect);

	// パイプラインステートの設定
	sCommandList_->SetPipelineState(sPipelineStates_.Get());
	// ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	// プリミティブ形状の設定を行う
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

/// <summary>
/// 描画後処理
/// </summary>
void Sprite::PostDraw() {

	// 取得したコマンドリストを削除する
	Sprite::sCommandList_ = nullptr;

}

/// <summary>
/// DXCの初期化関数
/// </summary>
void Sprite::InitializeDXC() {

	// 結果確認用
	HRESULT result = S_FALSE;

	/// dxcCompilerを初期化
	// dxcUtilsを生成する
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	// 作成出来ているかを確認する
	assert(SUCCEEDED(result));
	// dxcCompilerを生成する
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	// 作成できるかを確認する
	assert(SUCCEEDED(result));

	// include に対応させるための設定を行う
	result = dxcUtils_->CreateDefaultIncludeHandler(&dxcIncludeHandler_);
	// 設定を行えたか確認する
	assert(SUCCEEDED(result));

}

/// <summary>
/// シェーダーのコンパイルを行う関数
/// </summary>
/// <param name="filePath">compilerするSharderファイルへのパス</param>
/// <param name="profile">compilerに使用するprofile</param>
/// <returns>コンパイル済みシェーダー</returns>
IDxcBlob* Sprite::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile) {

	// 結果確認用
	HRESULT result = S_FALSE;

	// シェーダーコンパイルの開始をログに出す
	Debug::Log(Debug::ConvertString(std::format(L"Begin CompileShader, path:{}, profile{}\n", filePath, profile)));

	// HLSLファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読み込み出来ているかを確認する
	assert(SUCCEEDED(result));

	// 読み込んだファイルの内容の設定を行う
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	// UTF-8形式の文字コードとして設定する
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	// コンパイルオプションの設定を行う
	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定 基本的にmain以外にはしない
		L"-T", profile, // shaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバック用の情報埋め込み
		L"-Od", // 最適化を外す
		L"-Zpr", // メモリレイアウトは行優先
	};

	// 実際にシェーダーのコンパイルを行う
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,        // 読み込んだファイル
		arguments,                  // コンパイルオプション
		_countof(arguments),        // コンパイルオプションの数
		dxcIncludeHandler_.Get(),   // include が含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);

	// ここで吐くエラーはコンパイルエラーではなく、dxcが起動できないなどの致命的なもの
	assert(SUCCEEDED(result));

	// 警告やエラーが出た場合にはログに出して停止させる
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// ログに警告、エラー情報を出力する
		Debug::Log(shaderError->GetStringPointer());
		// 警告やエラーが出ている場合停止させる
		assert(false);
	}

	// コンパイル結果から実行用のバイナリ部分を取得する
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	// 取得出来たか確認する
	assert(SUCCEEDED(result));

	// 成功したことをログに出力
	Debug::Log(Debug::ConvertString(std::format(L"Compile Succeeded, path:{}, profile{}\n", filePath, profile)));

	// 使わないリソースの解放を行う
	shaderSource->Release();
	shaderResult->Release();

	// 実行用のバイナリを返す
	return shaderBlob;

}

/// <summary>
/// バッファリソース作成関数
/// </summary>
/// <param name="device">作成に使用するデバイス</param>
/// <param name="sizeInBytes">サイズ</param>
/// <returns></returns>
ComPtr<ID3D12Resource> Sprite::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	// 結果確認用
	HRESULT result = S_FALSE;

	// 頂点リソース用のヒープを設定する
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ　今回はVector4を3頂点分(怪しい)

	// バッファの場合これらを1にする
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	// バッファの場合これにする
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際に頂点リソースを作成
	ComPtr<ID3D12Resource>vertexResource = nullptr;
	result = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));

	assert(SUCCEEDED(result));

	return vertexResource;

}

/// <summary>
/// スプライト生成関数
/// </summary>
/// <param name="position">座標</param>
/// <param name="color">色</param>
/// <param name="anchorPoint">アンカーポイント</param>
/// <returns>生成されたスプライト</returns>
Sprite* Sprite::Create(
	Vector2 position, Vector4 color,
	Vector2 anchorPoint
) {

	// 仮のサイズ
	Vector2 size = { 0.5f, 0.5f };

	// スプライトのインスタンスを生成する
	Sprite* sprite =
		new Sprite(position, size, color, anchorPoint);
	// スプライトの中身がnullならnullを返す
	if (sprite == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!sprite->Initialize()) {
		// 作成したスプライトを削除する
		delete sprite;
		assert(0);
		// nullを返す
		return nullptr;
	}

	// スプライトを返す
	return sprite;

}

/// <summary>
/// コンストラクタ
/// </summary>
Sprite::Sprite() {}

/// <summary>
/// コンストラクタ
/// </summary>
Sprite::Sprite(Vector2 position, Vector2 size,
	Vector4 color, Vector2 anchorPoint) {

	// 引数の値をメンバ変数に代入する
	transform_.translate.x = position.x;
	transform_.translate.y = position.y;
	transform_.scale.x = size.x;
	transform_.scale.y = size.y;
	anchorPoint_ = anchorPoint;
	matWorld_ = MyMath::MakeIdentity4x4();
	color_ = color;

}

/// <summary>
/// 初期化
/// </summary>
/// <returns>初期化出来ているか</returns>
bool Sprite::Initialize() {

	// NULLチェック
	assert(sDevice_);

	// 結果確認用
	HRESULT result = S_FALSE;

	// 頂点バッファリソース生成
	vertBuff_ = CreateBufferResource(sDevice_, sizeof(VertexData) * 3);
	// 頂点バッファのマッピングを行う
	result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));
	// マッピング出来ているかを確認する
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	TransferVertices();

	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 使用するリソースのサイズを設定する
	vbView_.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点アドレスを設定する
	vbView_.StrideInBytes = sizeof(VertexData);

	// 定数バッファリソース作成
	constBuff_ = CreateBufferResource(sDevice_, sizeof(MaterialData));
	// 定数バッファのマッピングを行う
	result = constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
	// マッピングできているかを確認する
	assert(SUCCEEDED(result));

	// 初期化が完了したらTrueを返す
	return true;

}

/// <summary>
/// 描画関数
/// </summary>
/// <param name="vpMatrix">ビュープロジェクション行列</param>
void Sprite::Draw(Matrix4x4 vpMatrix) {

	// 行列変換
	Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matWorld_ = MyMath::Multiply(worldMatrix, vpMatrix);

	// 色を設定
	constMap_->color = color_;

	// 頂点バッファの設定
	sCommandList_->IASetVertexBuffers(0, 1, &vbView_);
	// 定数バッファの設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// 描画コマンド
	sCommandList_->DrawInstanced(3, 1, 0, 0);

}

/// <summary>
/// 頂点データ転送関数
/// </summary>
void Sprite::TransferVertices() {

	// 頂点
	enum {
		LB, // 左下
		T, // 上
		RB, // 右下
	};

	// 4頂点の座標を設定
	/*float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;*/

	// 頂点データ
	VertexData vertices[kVertexNum];

	vertices[LB].position = { -transform_.scale.x, -transform_.scale.y, 0.0f, 1.0f };  // 左下
	vertices[T].position = { 0.0f, transform_.scale.y, 0.0f, 1.0f };     // 左上
	vertices[RB].position = { transform_.scale.x, -transform_.scale.y, 0.0f, 1.0f }; // 右下

	// 頂点バッファへのデータ転送を行う
	memcpy(vertMap_, vertices, sizeof(vertices));

}

