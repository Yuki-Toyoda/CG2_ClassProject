#pragma once
#include <vector>
#include "DirectXCommon.h"
#include "LightGroup.h"
#include "Triangle.h"
#include "Sprite.h"
#include "Sphere.h"
#include "Model.h"
#include "externals/imgui/imgui.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private: // メンバ変数

	// DirectX汎用クラス
	DirectXCommon* dxCommon_ = nullptr;

	// ImGui用ライト角度
	Vector3 setRotation_ = { 0.0f, -1.0f, 0.0f };
	// ImGui用ライト輝度
	float setIntensity_ = 1.0f;
	// ImGui用ライト有効フラグ
	bool enableLighting_ = true;

	// カメラ座標
	Vector3 translate_ = { 0.0f, 0.0f, -10.0f };
	// カメラ拡大率
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// カメラ回転角
	Vector3 rotate_ = { 0.0f, 0.0f, 0.0f };

	// カメラ行列
	Matrix4x4 cameraMatrix_;
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	// 三角形
	std::vector<Triangle*> triangles_;
	// 三角形最大数
	static const int kMaxTriangles_ = 3;
	int32_t isDeleteTriangles_[kMaxTriangles_] = { false };

	// スプライト
	std::vector<Sprite*> sprites_;
	// スプライト最大数
	static const int kMaxSprites_ = 5;
	int32_t isDeleteSprites_[kMaxSprites_] = { false };

	// 球
	std::vector<Sphere*> spheres_;
	// 球最大数
	static const int kMaxSpheres_ = 3;
	int32_t isDeletespheres_[kMaxSpheres_] = { false };

	// obj
	std::vector<Model*> models_;
	// obj最大数
	static const int kMaxModels_ = 5;
	int32_t isDeleteModels_[kMaxModels_] = { false };

	// テクスチャ
	uint32_t sampleTextureHandle_ = 0u;

	// テクスチャ群
	std::vector<uint32_t> textureHandles_;
	// テクスチャ最大数
	static const int kMaxTextures_ = 10;
	int32_t isDeleteTextures_[kMaxTextures_] = { false };
	// ImGui用変数群
	Vector3 spawnPositon_ = { 0.0f, 0.0f, 0.0f }; // オブジェクトスポーン座標
	char loadTextureName_[32] = "";
	int selectingTexture_ = 999;

	char loadModelDirectoryPath_[32] = "./Resources";
	char loadModelFileName_[32] = "";
};