#include "GameScene.h"
#include "TextureManager.h"
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

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");
	textureHandle2_ = TextureManager::Load("white1x1.png");

	// スプライトの初期化
	triangle_ = Triangle::Create(textureHandle_, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	triangle_->SetSize({1.0f, 1.0f});
	triangle2_ = Triangle::Create(textureHandle2_, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	triangle2_->SetSize({ 1.0f, 0.5f });

	sprite_ = Sprite::Create(textureHandle_, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	sprite_->SetSize({ 1.25f, 1.0f });
	sprite_->SetPosition({ 2.0f, 1.0f });
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

	rotation.y += 0.01f;

	triangle_->SetRotation(rotation);

	// カメラ更新
	cameraMatrix_ = MyMath::MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = MyMath::Inverse(cameraMatrix_);
	projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kwindowHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

	Vector3 rotationCheck = triangle_->GetRotation();

	ImGui::Begin("Triangle");
	ImGui::DragFloat3("rotation", &rotationCheck.x, 0.01f);
	ImGui::End();

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 三角形描画前処理
	Triangle::PreDraw(commandList);
	// スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 三角形描画
	triangle_->Draw(viewProjectionMatrix_);
	triangle2_->Draw(viewProjectionMatrix_);

	// スプライト描画
	sprite_->Draw(viewProjectionMatrix_);

	// 三角形描画後処理
	Triangle::PostDraw();
	// スプライト描画後処理
	Sprite::PostDraw();

}