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

	// 三角形の初期化
	triangle_ = Triangle::Create(textureHandle_, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	triangle_->SetSize({1.0f, 1.0f});
	triangle2_ = Triangle::Create(textureHandle2_, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	triangle2_->SetSize({ 1.0f, 0.5f });
	// スプライトの初期化
	sprite_ = Sprite::Create(textureHandle_, { 10.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	sprite_->SetSize({ 100.0f, 100.0f });
	sprite_->SetPosition({ 360.0f, 240.0f });

	// スフィア初期化
	sphere_ = Sphere::Create(textureHandle_, { 0.0f, 0.0f, 5.0f }, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	sphere2_ = Sphere::Create(textureHandle_, { 3.0f, 0.0f, 5.0f }, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

	// モデル読み込み
	model_ = Model::Create(textureHandle_, { 0.0f, 0.0f, 5.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "./Resources", "plane.obj");
	model2_ = Model::Create(textureHandle2_, { 0.0f, 0.0f, 5.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "./Resources", "axis.obj");
	model_->SetPosition({ -1.0f, 0.0f, 0.0f });

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

	// カメラ更新
	cameraMatrix_ = MyMath::MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = MyMath::Inverse(cameraMatrix_);
	projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kwindowHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

	// カメラ移動
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Translation", &translate_.x, 0.05f);
	ImGui::DragFloat3("rotation", &rotate_.x, 0.05f);
	ImGui::End();

	#pragma region 三角形

	Vector3 transformChangeTriangle = triangle_->GetPosition();

	Vector3 rotationChangeTriangle = triangle_->GetRotation();

	if (rotationChangeTriangle.x >= (float)std::numbers::pi * 2.0f || rotationChangeTriangle.x <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeTriangle.x = 0.0f;
	}

	if (rotationChangeTriangle.y >= (float)std::numbers::pi * 2.0f || rotationChangeTriangle.y <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeTriangle.y = 0.0f;
	}

	if (rotationChangeTriangle.z >= (float)std::numbers::pi * 2.0f || rotationChangeTriangle.z <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeTriangle.z = 0.0f;
	}

	ImGui::Begin("Triangle");
	ImGui::DragFloat3("translation", &transformChangeTriangle.x, 0.01f);
	ImGui::DragFloat3("rotation", &rotationChangeTriangle.x, 0.01f);
	ImGui::End();
	triangle_->SetPosition(transformChangeTriangle);
	triangle_->SetRotation(rotationChangeTriangle);

#pragma endregion

	#pragma region モデル

	Vector3 transformChangeModel = model_->GetPosition();

	Vector3 rotationChangeModel = model_->GetRotation();

	if (rotationChangeModel.x >= (float)std::numbers::pi * 2.0f || rotationChangeModel.x <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeModel.x = 0.0f;
	}

	if (rotationChangeModel.y >= (float)std::numbers::pi * 2.0f || rotationChangeModel.y <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeModel.y = 0.0f;
	}

	if (rotationChangeModel.z >= (float)std::numbers::pi * 2.0f || rotationChangeModel.z <= -(float)std::numbers::pi * 2.0f) {
		rotationChangeModel.z = 0.0f;
	}

	ImGui::Begin("Model");
	ImGui::DragFloat3("translation", &transformChangeModel.x, 0.01f);
	ImGui::DragFloat3("rotation", &rotationChangeModel.x, 0.01f);
	ImGui::End();
	model_->SetPosition(transformChangeModel);
	model_->SetRotation(rotationChangeModel);

#pragma endregion

	#pragma region Light
	// ライトデバック
	ImGui::Begin("Light");
	ImGui::Checkbox("lightActive", &enableLighting_);
	ImGui::DragFloat3("Direction", &setRotation_.x, 0.05f, -1.0f, 1.0f);
	ImGui::DragFloat("Intensity", &setIntensity_, 0.05f, 0.0f, 5.0f);
	ImGui::End();

	// 球ライト設定
	Sphere::SetLightActive(0, enableLighting_);
	Sphere::SetLightRotation(0, setRotation_);
	Sphere::SetLightIntensity(0, setIntensity_);
	#pragma endregion

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 三角形描画前処理
	Triangle::PreDraw(commandList);

	// 三角形描画
	triangle_->Draw(viewProjectionMatrix_);
	//triangle2_->Draw(viewProjectionMatrix_);

	// 三角形描画後処理
	Triangle::PostDraw();

	Model::PreDraw(commandList);

	model_->Draw(viewProjectionMatrix_);
	model2_->Draw(viewProjectionMatrix_);

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(commandList);

	// スプライト描画
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

	// 球描画前処理
	Sphere::PreDraw(commandList);

	// 球描画
	sphere_->Draw(viewProjectionMatrix_);
	sphere2_->Draw(viewProjectionMatrix_);

	// 球描画後処理
	Sphere::PostDraw();

}