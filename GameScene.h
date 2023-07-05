#pragma once
#include "DirectXCommon.h"
#include "Sprite.h"

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

	// カメラ座標
	Vector3 translate_ = { 0.0f, 0.0f, -100.0f };
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

	// スプライト
	Sprite* sprite_ = nullptr;

};

