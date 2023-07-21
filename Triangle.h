#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include "DirectXCommon.h"
#include "MyMath.h"
#include "MyStruct.h"
#include "Debug.h"

#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// 三角形描画クラス
/// </summary>
class Triangle
{
public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化関数
	/// </summary>
	/// <param name="device">DXGIデバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// スプライト生成関数
	/// </summary>
	/// <param name="position">座標</param>
	/// <param name="color">色</param>
	/// <param name="anchorPoint">アンカーポイント</param>
	/// <returns>生成されたスプライト</returns>
	static Triangle* Create(
		Vector3 position, Vector2 size = {1.0f, 1.0f}, Vector4 color = {1,1,1,1},
		Vector2 anchorPoint = { 0.0f, 0.0f }
	);

	/// <summary>
	/// DXCの初期化関数
	/// </summary>
	static void InitializeDXC();

	/// <summary>
	/// シェーダーのコンパイルを行う関数
	/// </summary>
	/// <param name="filePath">compilerするSharderファイルへのパス</param>
	/// <param name="profile">compilerに使用するprofile</param>
	/// <param name="dxcUtils">dxcUtils</param>
	/// <param name="dxcCompiler">dxcCompiler</param>
	/// <param name="includeHandler">includeHandler</param>
	/// <returns>コンパイル済みシェーダー</returns>
	static IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>
	/// バッファリソース作成関数
	/// </summary>
	/// <param name="device">作成に使用するデバイス</param>
	/// <param name="sizeInBytes">サイズ</param>
	/// <returns></returns>
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

private: // 静的なメンバ変数

	// 頂点数
	static const int kVertexNum = 3;
	// デバイス
	static ID3D12Device* sDevice_;

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineStates_;

	// dxcUtils
	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	// dxcコンパイラ
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	// InludeHandler
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> dxcIncludeHandler_;

	// 正射影行列
	static Matrix4x4 sMatProjection_;

public: // メンバ関数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Triangle();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Triangle(Vector3 position, Vector2 size,
		Vector4 color, Vector2 anchorPoint);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>初期化出来ているか</returns>
	bool Initialize();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="vpMatrix">ビュープロジェクション行列</param>
	void Draw(Matrix4x4 vpMatrix);

	/// <summary>
	/// ポジションのセッター
	/// </summary>
	/// /// <param name="translation">セットする座標</param>
	void SetPosition(const Vector3& translation) { transform_.translate = translation;}
	/// <summary>
	/// 回転角のセッター
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& rotation) { 
		transform_.rotate = rotation;
		TransferVertices();
	}

private: // メンバ変数

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// 頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	// 定数バッファマップ
	MaterialData* constMap_ = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	// 構造体
	MyTransform transform_{};

	// スプライトの幅と高さ
	Vector2 size_ = { 100.0f, 100.0f };
	// 回転角
	float rotation_ = 0.0f;
	// アンカーポイント
	Vector2 anchorPoint_ = { 0, 0 };
	// ワールド行列
	Matrix4x4 matWorld_{};
	
	// 色
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

private: // メンバ関数

	/// <summary>
	/// 頂点データ転送関数
	/// </summary>
	void TransferVertices();

};

