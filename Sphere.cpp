#include "Sphere.h"
#include <cassert>
#include <d3dcompiler.h>
#include "TextureManager.h"

#pragma comment(lib, "d3dCompiler.lib")

using namespace Microsoft::WRL;

// 静的なメンバ変数の実体を宣言する

// 定数宣言
const uint32_t Sphere::kSubdivison = 16;												// 分割数
const float Sphere::kLonEvery = 2.0f * float(std::numbers::pi) / float(kSubdivison);	// 経度1つ分の分割角度
const float Sphere::kLatEvery = float(std::numbers::pi) / float(kSubdivison);			// 経度1つ分の分割角度
const uint32_t Sphere::kVertexIndex = kSubdivison * kSubdivison * 6;					// 頂点数

// デバイス
ID3D12Device* Sphere::sDevice_ = nullptr;

// コマンドリスト
ID3D12GraphicsCommandList* Sphere::sCommandList_ = nullptr;
// ルートシグネチャ
ComPtr<ID3D12RootSignature> Sphere::sRootSignature_;
// パイプラインステートオブジェクト
ComPtr<ID3D12PipelineState> Sphere::sPipelineStates_;

// dxcUtils
ComPtr<IDxcUtils> Sphere::dxcUtils_ = nullptr;
// dxcコンパイラ
ComPtr<IDxcCompiler3> Sphere::dxcCompiler_ = nullptr;
// InludeHandler
ComPtr<IDxcIncludeHandler> Sphere::dxcIncludeHandler_ = nullptr;

// 正射影行列
Matrix4x4 Sphere::sMatProjection_;

// ライト
std::unique_ptr<LightGroup> Sphere::lightGroup_;

void Sphere::StaticInitialize(ID3D12Device* device)
{
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

	// DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// ルートシグネチャを作成する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// ルートパラメータを設定する
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 1;
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	// Sampler設定
	D3D12_STATIC_SAMPLER_DESC staticSamples[1] = {};
	staticSamples[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamples[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamples[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamples[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamples[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamples[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipMapをつかう
	staticSamples[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamples[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamples;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamples);

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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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

	// デプスステンシルビューの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// 深度の機能を有効
	depthStencilDesc.DepthEnable = true;
	// 書き込む
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はlessEqual 近ければ描画
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// 頂点シェーダ用のバイナリオブジェクト
	ComPtr<IDxcBlob> vertexBlob;
	// ピクセルシェーダ用のバイナリオブジェクト
	ComPtr<IDxcBlob> pixelBlob;

	// 頂点シェーダをコンパイルする
	vertexBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0");
	// コンパイル出来たか確認する
	assert(vertexBlob != nullptr);
	// ピクセルシェーダをコンパイルする
	pixelBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0");
	// コンパイル出来たか確認する
	assert(pixelBlob != nullptr);

	// ルートシグネチャ
	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();
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
	// デプスステンシルビュー
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

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

	// ライト生成
	lightGroup_.reset(LightGroup::Create());

}

void Sphere::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていない場合はエラー
	assert(Sphere::sCommandList_ == nullptr);

	// コマンドリストをセットする
	sCommandList_ = cmdList;

	// ビューポート設定
	sCommandList_->RSSetViewports(1, DirectXCommon::GetInstance()->GetViewPort());
	// シザー矩形設定
	sCommandList_->RSSetScissorRects(1, DirectXCommon::GetInstance()->GetScissorRect());

	// パイプラインステートの設定
	sCommandList_->SetPipelineState(sPipelineStates_.Get());
	// ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	// プリミティブ形状の設定を行う
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Sphere::PostDraw()
{
	// 取得したコマンドリストを削除する
	Sphere::sCommandList_ = nullptr;
}

Sphere* Sphere::Create(uint32_t textureHandle, Vector3 position, float radius, Vector4 color)
{
	// スプライトのインスタンスを生成する
	Sphere* sphere =
		new Sphere(textureHandle, position, radius, color);
	// スプライトの中身がnullならnullを返す
	if (sphere == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!sphere->Initialize()) {
		// 作成したスプライトを削除する
		delete sphere;
		assert(0);
		// nullを返す
		return nullptr;
	}

	// スプライトを返す
	return sphere;
}

void Sphere::InitializeDXC()
{
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

IDxcBlob* Sphere::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
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

Microsoft::WRL::ComPtr<ID3D12Resource> Sphere::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
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

Sphere::Sphere(){}

Sphere::Sphere(uint32_t textureHandle, Vector3 position, float radius, Vector4 color)
{
	radius;
	// 引数の値をメンバ変数に代入する
	transform_.translate = position;
	transform_.scale.x = radius;
	transform_.scale.y = radius;
	transform_.scale.z = radius;
	matWorld_ = MyMath::MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
}

bool Sphere::Initialize()
{
	// NULLチェック
	assert(sDevice_);

	// 結果確認用
	HRESULT result = S_FALSE;

	// テクスチャのリソース情報を取得
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

	// 頂点バッファリソース生成
	vertBuff_ = CreateBufferResource(sDevice_, sizeof(VertexData) * kVertexIndex);
	// 頂点バッファのマッピングを行う
	result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap_));
	// マッピング出来ているかを確認する
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	TransferVertices();

	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 使用するリソースのサイズを設定する
	vbView_.SizeInBytes = sizeof(VertexData) * kVertexIndex;
	// 1頂点アドレスを設定する
	vbView_.StrideInBytes = sizeof(VertexData);

	// 定数バッファリソース作成
	constBuff_ = CreateBufferResource(sDevice_, sizeof(ConstBufferData));
	// 定数バッファのマッピングを行う
	result = constBuff_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
	// マッピングできているかを確認する
	assert(SUCCEEDED(result));

	// 初期化が完了したらTrueを返す
	return true;
}

void Sphere::Draw(Matrix4x4 vpMatrix)
{

	lightGroup_->Update();

	// ライトの描画
	lightGroup_->Draw(sCommandList_, 2);

	// 行列変換
	Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matWorld_ = MyMath::Multiply(worldMatrix, vpMatrix);

	// 色を設定
	constMap_->color = color_;
	// 行列を設定
	constMap_->mat = matWorld_;
	constMap_->world = worldMatrix;
	// ライティングを有効
	constMap_->enableLighting = true;

	// 頂点バッファの設定
	sCommandList_->IASetVertexBuffers(0, 1, &vbView_);
	// 定数バッファの設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	TextureManager::GetInstance()->SetGraphicsDescriptorTable(sCommandList_, 1, textureHandle_);

	// 描画コマンド
	sCommandList_->DrawInstanced(kVertexIndex, 1, 0, 0);

}

void Sphere::SetTextureHandle(uint32_t textureHandle)
{
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
}

void Sphere::TransferVertices()
{
	// 頂点データ
	VertexData vertices[kVertexIndex];

	const float dTheta = (float)std::numbers::pi / kSubdivison;
	const float dPhi = 2 * (float)std::numbers::pi / kSubdivison;
	// 緯度の方向に分割 -π / 2 ～ π / 2
	for (uint32_t latIndex = 0; latIndex < kSubdivison; latIndex++) {
		// 現在の緯度
		float lat = -(float)std::numbers::pi / 2.0f + kLatEvery * latIndex;
		// 経度の方向に分割 0 ～ 2 π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivison; lonIndex++) {
			// 現在の経度
			float lon = lonIndex * kLonEvery;
			// world 座標系での a, b, c を求める
			Vector4 pA, pB, pC, pD;
			// texcoord
			Vector2 tA, tB, tC, tD;
			tA.x = float(lonIndex) / float(kSubdivison);
			tA.y = 1.0f - float(latIndex) / float(kSubdivison);
			tB.x = float(lonIndex) / float(kSubdivison);
			tB.y = 1.0f - float(latIndex + 1) / float(kSubdivison);
			tC.x = float(lonIndex + 1) / float(kSubdivison);
			tC.y = 1.0f - float(latIndex) / float(kSubdivison);
			tD.x = float(lonIndex + 1) / float(kSubdivison);
			tD.y = 1.0f - float(latIndex + 1) / float(kSubdivison);
			// a, b, c を Screen 座標系まで変換
			pA = { cosf(lat) * cosf(lon),sinf(lat),cosf(lat) * sinf(lon),1.0f };
			pB = { cosf(lat + dTheta) * cosf(lon),sinf(lat + dTheta),cosf(lat + dTheta) * sinf(lon) ,1.0f };
			pC = { cosf(lat) * cosf(lon + dPhi),sinf(lat),cosf(lat) * sinf(lon + dPhi) ,1.0f };
			pD = { cosf(lat + dTheta) * cosf(lon + dPhi),sinf(lat + dTheta),cosf(lat + dTheta) * sinf(lon + dPhi) ,1.0f };

			uint32_t startIndex = (latIndex * kSubdivison + lonIndex) * 6;

			vertices[startIndex].position = pA;
			vertices[startIndex].uv = tA;
			vertices[startIndex].normal.x = vertices[startIndex].position.x;
			vertices[startIndex].normal.y = vertices[startIndex].position.y;
			vertices[startIndex].normal.z = vertices[startIndex].position.z;

			vertices[startIndex + 1].position = pB;
			vertices[startIndex + 1].uv = tB;
			vertices[startIndex + 1].normal.x = vertices[startIndex + 1].position.x;
			vertices[startIndex + 1].normal.y = vertices[startIndex + 1].position.y;
			vertices[startIndex + 1].normal.z = vertices[startIndex + 1].position.z;

			vertices[startIndex + 2].position = pC;
			vertices[startIndex + 2].uv = tC;
			vertices[startIndex + 2].normal.x = vertices[startIndex + 2].position.x;
			vertices[startIndex + 2].normal.y = vertices[startIndex + 2].position.y;
			vertices[startIndex + 2].normal.z = vertices[startIndex + 2].position.z;

			vertices[startIndex + 3].position = pC;
			vertices[startIndex + 3].uv = tC;
			vertices[startIndex + 3].normal.x = vertices[startIndex + 3].position.x;
			vertices[startIndex + 3].normal.y = vertices[startIndex + 3].position.y;
			vertices[startIndex + 3].normal.z = vertices[startIndex + 3].position.z;

			vertices[startIndex + 4].position = pB;
			vertices[startIndex + 4].uv = tB;
			vertices[startIndex + 4].normal.x = vertices[startIndex + 4].position.x;
			vertices[startIndex + 4].normal.y = vertices[startIndex + 4].position.y;
			vertices[startIndex + 4].normal.z = vertices[startIndex + 4].position.z;

			vertices[startIndex + 5].position = pD;
			vertices[startIndex + 5].uv = tD;
			vertices[startIndex + 5].normal.x = vertices[startIndex + 5].position.x;
			vertices[startIndex + 5].normal.y = vertices[startIndex + 5].position.y;
			vertices[startIndex + 5].normal.z = vertices[startIndex + 5].position.z;

		}
	}

	// 頂点バッファへのデータ転送
	memcpy(vertMap_, vertices, sizeof(vertices));

}
