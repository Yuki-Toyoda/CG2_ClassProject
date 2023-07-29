#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <vector>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include <numbers>
#include <fstream>
#include <istream>
#include "DirectXCommon.h"
#include "LightGroup.h"
#include "MyMath.h"
#include "MyStruct.h"
#include "Debug.h"

class Model
{
private: // サブクラス

	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexData {
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
	};
	struct MaterialData {
		// テクスチャまでのファイルパス
		std::string textureFilePath;
	};

	/// <summary>
	/// モデルデータ構造体
	/// </summary>
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	/// <summary>
	/// 定数バッファデータ構造体
	/// </summary>
	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 mat;
		Matrix4x4 world;
		int32_t enableLighting;
	};

public: // メンバ関数

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
	/// モデル生成関数
	/// </summary>
	/// <param name="textureHandle">テクスチャ</param>
	/// <param name="position">初期座標</param>
	/// <param name="radius">球半径</param>
	/// <param name="subdivison">分割数</param>
	/// <param name="color">色</param>
	/// <returns>生成出来たか</returns>
	static Model* Create(
		uint32_t textureHandle, Vector3 position, Vector4 color = { 1,1,1,1 }, const std::string& directoryPath = "./Resources", const std::string& fileName = ".obj");

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

	static void SetLightActive(int index, bool active) { lightGroup_->SetDirectionalLightActive(index, active); }

	/// <summary>
	/// ライト角度のセッター
	/// </summary>
	static void SetLightRotation(int index, Vector3 rotation) { lightGroup_->SetDirectionalLightDirection(index, rotation); }

	/// <summary>
	/// ライト輝度セッター
	/// </summary>
	/// <param name="intensity"></param>
	static void SetLightIntensity(int index, float intensity) { lightGroup_->SetDirectionalLightIntensity(index, intensity); }

private: // 静的なメンバ変数

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

	// ライト
	static std::unique_ptr<LightGroup> lightGroup_;

public: // メンバ関数

	/// <summary>
	/// objファイル読み込み関数
	/// </summary>
	/// <param name="directoryPath">モデルまでのファイルパス</param>
	/// <param name="fileName">ファイル名</param>
	/// <returns>モデルデータ</returns>
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& fileName);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Model();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Model(uint32_t textureHandle, Vector3 position, Vector4 color = { 1,1,1,1 }
	);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>初期化出来ているか</returns>
	bool Initialize(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="vpMatrix">ビュープロジェクション行列</param>
	void Draw(Matrix4x4 vpMatrix);

	/// <summary>
	/// スプライト中心座標のセッター
	/// </summary>
	/// /// <param name="translation">セットする座標</param>
	void SetPosition(const Vector3& translation) {
		transform_.translate = translation;
		TransferVertices();
	}
	/// <summary>
	/// スプライト中心座標のゲッター
	/// </summary>
	/// <returns>中心座標</returns>
	Vector3 GetPosition() { return transform_.translate; }

	/// <summary>
	/// 回転角のセッター
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& rotation) {
		transform_.rotate = rotation;
		TransferVertices();
	}
	/// <summary>
	/// 回転角のゲッター
	/// </summary>
	/// <returns></returns>
	Vector3 GetRotation() { return transform_.rotate; }

	/// <summary>
	/// サイズのセッター
	/// </summary>
	/// <param name="size">設定するサイズ</param>
	void SetSize(const Vector3& size) {
		transform_.scale = size;
		TransferVertices();
	}
	/// <summary>
	/// サイズのゲッター
	/// </summary>
	/// <returns>サイズ</returns>
	Vector3 GetSize() {
		return transform_.scale;
	}

	/// <summary>
	/// 色のセッター
	/// </summary>
	/// <param name="color">設定する色</param>
	void SetColor(Vector4 color) { color_ = color; }
	/// <summary>
	/// 色のゲッター
	/// </summary>
	/// <returns>色</returns>
	Vector4 GetColor() { return color_; }

private: // メンバ変数

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// 頂点バッファマップ
	VertexData* vertMap_ = nullptr;
	// 定数バッファマップ
	ConstBufferData* constMap_ = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	ModelData myModelData_; // 構築するモデルデータ

	// 構造体
	MyTransform transform_{};

	// 球半径
	float radius_;

	// ワールド行列
	Matrix4x4 matWorld_{};

	// テクスチャ
	uint32_t textureHandle_ = 0u;
	// テクスチャ始点
	Vector2 texBase_ = { 0.0f, 0.0f };
	// テクスチャ幅と高さ
	Vector2 texSize_ = { 100.0f, 100.0f };

	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc_;

	// 色
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

private: // メンバ関数

	/// <summary>
	/// 頂点データ転送関数
	/// </summary>
	void TransferVertices();

};

