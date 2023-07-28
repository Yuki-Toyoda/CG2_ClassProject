#pragma once
#include "DirectXCommon.h"
#include "LightGroup.h"
#include "Triangle.h"
#include "Sprite.h"
#include "Sphere.h"

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

	Vector3 setRotation_ = { 0.0f, -1.0f, 0.0f };

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
	Triangle* triangle_ = nullptr;
	Triangle* triangle2_ = nullptr;

	// スプライト
	Sprite* sprite_ = nullptr;

	// 球
	Sphere* sphere_ = nullptr;
	Sphere* sphere2_ = nullptr;

	// テクスチャ
	uint32_t textureHandle_ = 0u;
	uint32_t textureHandle2_ = 0u;

	Vector3 rotation = { 0.0f, 0.0f, 0.0f };

};

