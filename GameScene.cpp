#include "GameScene.h"
#include "cassert"

/// <summary>
/// コンストラクタ
/// </summary>
GameScene::GameScene() {};

/// <summary>
/// デストラクタ
/// </summary>
GameScene::~GameScene() {};

/// <summary>
/// 初期化処理
/// </summary>
void GameScene::Initialize() {
	
	// DirectX汎用クラスのインスタンスを取得
	dxCommon_ = DirectXCommon::GetInstance();

	// スプライトの初期化
	sprite_ = Sprite::Create({ 0.0f, 0.0f }, { 1, 1, 1, 1 }, { 0.0f, 0.0f });

	// カメラ初期化
	cameraMatrix_ = MyMath::MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = MyMath::Inverse(cameraMatrix_);
	projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kwindowHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);
}

/// <summary>
/// 更新処理
/// </summary>
void GameScene::Update() {

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// スプライト描画前処理
	Sprite::PreDraw(commandList);

	// スプライト描画
	sprite_->Draw(viewProjectionMatrix_);

	// スプライト描画後処理
	Sprite::PostDraw();

}