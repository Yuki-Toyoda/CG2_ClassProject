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
GameScene::~GameScene() {
};

/// <summary>
/// 初期化処理
/// </summary>
void GameScene::Initialize() {
	
	// DirectX汎用クラスのインスタンスを取得
	dxCommon_ = DirectXCommon::GetInstance();

	sampleTextureHandle_ = TextureManager::Load("white1x1.png");

	// カメラ初期化
	cameraMatrix_ = MyMath::MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = MyMath::Inverse(cameraMatrix_);
	projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kwindowHeight), 0.1f, 100.0f);
	viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);
	
	// white1x1.pngの読み込み
	uint32_t texture = TextureManager::Load("white1x1.png");
	textureHandles_.push_back(texture);
	// UVChecker.pngの読み込み
	texture = TextureManager::Load("uvChecker.png");
	textureHandles_.push_back(texture);

	// 三角形を生成
	Triangle* tempTriangle = Triangle::Create(textureHandles_[0], {2.5f, 0.5f, 1.0f}, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	// リストに加える
	triangles_.push_back(tempTriangle);
	// 三角形を生成
	tempTriangle = Triangle::Create(textureHandles_[1], { 3.0f, 1.25f, 2.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
	tempTriangle->SetSize({ 2.0f, 2.0f });
	// リストに加える
	triangles_.push_back(tempTriangle);

	// スプライトを生成
	Sprite* tempSprite = Sprite::Create(textureHandles_[0], {250.0f, 100.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	tempSprite->SetSize({ 100.0f, 100.0f });
	// リストに加える
	sprites_.push_back(tempSprite);
	tempSprite = Sprite::Create(textureHandles_[1], {200.0f, 300.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	tempSprite->SetSize({ 256.0f, 256.0f });
	// リストに加える
	sprites_.push_back(tempSprite);

	// 球を生成
	Sphere* tempSphere = Sphere::Create(textureHandles_[1], { 0.5f, 0.5f, 3.0f}, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	// リストに加える
	spheres_.push_back(tempSphere);
	// 球を生成
	tempSphere = Sphere::Create(textureHandles_[0], { -1.5f, 1.0f, 5.0f }, 2.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	// リストに加える
	spheres_.push_back(tempSphere);

	// モデルを生成
	Model* tempModel = Model::Create(textureHandles_[0], {2.0f, -1.0f, 0.0f}, { 1.0f, 1.0f, 1.0f, 1.0f }, "./Resources", "teapot.obj");
	tempModel->SetRotation({ 0.0f, 0.5f, 0.0f });
	// リストに加える
	models_.push_back(tempModel);
	tempModel = Model::Create(textureHandles_[0], { -0.5f, -1.75f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "./Resources", "bunny.obj");
	tempModel->SetRotation({ 0.0f, -0.5f, 0.0f });
	// リストに加える
	models_.push_back(tempModel);
	tempModel = Model::Create(textureHandles_[0], { -2.5f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, "./Resources", "plane.obj");
	tempModel->SetRotation({ 0.0f, -0.5f, 0.0f });
	// リストに加える
	models_.push_back(tempModel);

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

	// デバック関連ウィンドウ
	ImGui::Begin("Debug");

	#pragma region ライト
	// ライトデバック
	if (ImGui::TreeNode("Light")) {

		ImGui::Checkbox("lightActive", &enableLighting_);
		ImGui::DragFloat3("Direction", &setRotation_.x, 0.05f, -1.0f, 1.0f);
		ImGui::DragFloat("Intensity", &setIntensity_, 0.05f, 0.0f, 5.0f);

		// ライト設定
		Sphere::SetLightActive(0, enableLighting_);
		Model::SetLightActive(0, enableLighting_);
		Sphere::SetLightRotation(0, setRotation_);
		Model::SetLightRotation(0, setRotation_);
		Sphere::SetLightIntensity(0, setIntensity_);
		Model::SetLightIntensity(0, setIntensity_);

		ImGui::TreePop();
	}
	#pragma endregion	
	#pragma region カメラ
	if (ImGui::TreeNode("Camera")) {
		ImGui::DragFloat3("Translation", &translate_.x, 0.05f);
		ImGui::DragFloat3("rotation", &rotate_.x, 0.05f);
		ImGui::TreePop();
	}
#pragma endregion
	ImGui::NewLine();
	#pragma region テクスチャ読み込み
	if (ImGui::TreeNode("LoadTexture")) {
		ImGui::InputText("TextureName", loadTextureName_, sizeof(loadTextureName_));
		if (ImGui::Button("LoadTexture")) {
			uint32_t texture = TextureManager::Load(loadTextureName_);
			textureHandles_.push_back(texture);
		}
		ImGui::TreePop();
	}
#pragma endregion
	#pragma region テクスチャ一覧と削除
	if (ImGui::TreeNode("Textures")) {
		if (textureHandles_.size() == 0)
			ImGui::Text("NoTextures!");

		ImGui::RadioButton("Don't use texture", &selectingTexture_, 999);

		// 全てのテクスチャ表示
		for (int i = 0; i < textureHandles_.size(); i++) {
			std::string textureName = TextureManager::GetInstance()->GetTextureName(textureHandles_[i]);
			D3D12_GPU_DESCRIPTOR_HANDLE textureGpuHandle = TextureManager::GetInstance()->GetTextureGPUHandle(textureHandles_[i]);
			D3D12_RESOURCE_DESC textureDesc = TextureManager::GetInstance()->GetResourceDesc(textureHandles_[i]);

			if (ImGui::TreeNode(textureName.c_str())) {
				ImGui::RadioButton("UseThisTexture", &selectingTexture_, i);
				if (ImGui::Button("Delete This")) {
					isDeleteTextures_[i] = true;
				}
				ImGui::Text("GPU handle = %p", textureGpuHandle.ptr);
				ImGui::TreePop();
			}
		}

		// 消す
		for (int i = 0; i < kMaxTextures_; i++) {
			if (isDeleteTextures_[i]) {
				std::erase(textureHandles_, textureHandles_[i]);
				isDeleteTextures_[i] = false;
			}
		}

		ImGui::TreePop();
	}
#pragma endregion
	ImGui::NewLine();
	#pragma region オブジェクトのスポーン
	// オブジェクトスポーン座標の設定
	ImGui::DragFloat3("SpawnPosition", &spawnPositon_.x, 0.05f);

	// 3Dモデルのディレクトリ設定
	ImGui::InputText("DirectoryPath", loadModelDirectoryPath_, sizeof(loadModelDirectoryPath_));
	ImGui::InputText("FileName", loadModelFileName_, sizeof(loadModelFileName_));
	ImGui::NewLine();
	if (ImGui::BeginMenu("ObjectCreate")) {
		// 三角形の生成
		// 三角形の最大数に達していないか
		if (triangles_.size() < kMaxTriangles_) {
			// メニュー表示
			if (ImGui::MenuItem("Triangle")) {
				// テクスチャが存在しない場合
				if (selectingTexture_ == 999) {
					// 三角形を生成
					Triangle* triangle = Triangle::Create(sampleTextureHandle_, spawnPositon_, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
					// リストに加える
					triangles_.push_back(triangle);
				}
				else {
					// 三角形を生成
					Triangle* triangle = Triangle::Create(textureHandles_[selectingTexture_], spawnPositon_, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
					triangle->SetSize({ 1.0f, 1.0f });
					// リストに加える
					triangles_.push_back(triangle);
				}

			}
		}
		else {
			ImGui::Text("MaxTriangles!");
		}
		// スプライトの生成
		// スプライトの最大数に達していないか
		if (sprites_.size() < kMaxSprites_) {
			// メニュー表示
			if (ImGui::MenuItem("Sprite")) {
				// テクスチャが存在しない場合
				if (selectingTexture_ == 999) {
					// スプライトを生成
					Sprite* sprite = Sprite::Create(sampleTextureHandle_, {spawnPositon_.x, spawnPositon_.y}, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
					// リストに加える
					sprites_.push_back(sprite);
				}
				else {
					// スプライトを生成
					Sprite* sprite = Sprite::Create(textureHandles_[selectingTexture_], { spawnPositon_.x, spawnPositon_.y }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f });
					// リストに加える
					sprites_.push_back(sprite);
				}

			}
		}
		else {
			ImGui::Text("MaxSprites!");
		}
		// 球の生成
		// 球の最大数に達していないか
		if (spheres_.size() < kMaxSpheres_) {
			// メニュー表示
			if (ImGui::MenuItem("Sphere")) {
				// テクスチャが存在しない場合
				if (selectingTexture_ == 999) {
					// 球を生成
					Sphere* sphere = Sphere::Create(sampleTextureHandle_, spawnPositon_, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
					// リストに加える
					spheres_.push_back(sphere);
				}
				else {
					// 球を生成
					Sphere* sphere = Sphere::Create(textureHandles_[selectingTexture_], spawnPositon_, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
					// リストに加える
					spheres_.push_back(sphere);
				}

			}
		}
		else {
			ImGui::Text("MaxSpheres!");
		}
		// objの生成
		// objの最大数に達していないか
		if (models_.size() < kMaxModels_) {
			// メニュー表示
			if (ImGui::MenuItem("Model")) {
				// テクスチャが存在しない場合
				if (selectingTexture_ == 999) {
					// モデルを生成
					Model* model = Model::Create(sampleTextureHandle_, spawnPositon_, { 1.0f, 1.0f, 1.0f, 1.0f }, loadModelDirectoryPath_, loadModelFileName_);
					// リストに加える
					models_.push_back(model);
				}
				else {
					// モデルを生成
					Model* model = Model::Create(textureHandles_[selectingTexture_], spawnPositon_, { 1.0f, 1.0f, 1.0f, 1.0f }, loadModelDirectoryPath_, loadModelFileName_);
					// リストに加える
					models_.push_back(model);
				}

			}
		}
		else {
			ImGui::Text("MaxModels!");
		}
		ImGui::EndMenu();
	}
#pragma endregion
	ImGui::NewLine();
	#pragma region 三角形のデバック
	if (ImGui::TreeNode("Triangles")) {
		if (triangles_.size() == 0)
			ImGui::Text("NoTriangle!");
		
		// 全ての三角形の値変更
		for (int i = 0; i < triangles_.size(); i++) {

			// 三角形の座標取得
			Vector2 scale = triangles_[i]->GetSize();
			Vector3 rotation = triangles_[i]->GetRotation();
			Vector3 translation = triangles_[i]->GetPosition();
			Vector4 color = triangles_[i]->GetColor();

			if (rotation.x >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.x)
				rotation.x = 0.0f;
			if (rotation.y >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.y)
				rotation.y = 0.0f;
			if (rotation.z >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.z)
				rotation.z = 0.0f;

			char tree[16] = "";
			sprintf_s(tree, "Triangles%d", i);
			if (ImGui::TreeNode(tree)) {
				ImGui::DragFloat2("Scale", &scale.x, 0.05f, 0.0f);
				ImGui::DragFloat3("Rotation", &rotation.x, 0.05f);
				ImGui::DragFloat3("translation", &translation.x, 0.05f);
				ImGui::ColorEdit4("color", &color.x);

				if (selectingTexture_ != 999) {
					if (ImGui::Button("Change to selected texture")) {
						triangles_[i]->SetTextureHandle(selectingTexture_);
					}
				}

				if (ImGui::Button("Delete This")) {
					isDeleteTriangles_[i] = true;
				}

				ImGui::TreePop();
			}

			triangles_[i]->SetSize(scale);
			triangles_[i]->SetRotation(rotation);
			triangles_[i]->SetPosition(translation);
			triangles_[i]->SetColor(color);

		}

		// 消す
		for (int i = 0; i < kMaxTriangles_; i++) {
			if (isDeleteTriangles_[i]) {
				std::erase(triangles_, triangles_[i]);
				isDeleteTriangles_[i] = false;
			}
		}

		ImGui::TreePop();
	}
	#pragma endregion
	#pragma region スプライトのデバック
	if (ImGui::TreeNode("Sprites")) {
		if (triangles_.size() == 0)
			ImGui::Text("NoSprite!");

		// 全ての三角形の値変更
		for (int i = 0; i < sprites_.size(); i++) {

			// 三角形の座標取得
			Vector2 scale = sprites_[i]->GetSize();
			float rotation = sprites_[i]->GetRotation();
			if (rotation >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation)
				rotation = 0.0f;
			Vector2 translation = sprites_[i]->GetPosition();
			Vector4 color = sprites_[i]->GetColor();

			char tree[16] = "";
			sprintf_s(tree, "Sprites%d", i);
			if (ImGui::TreeNode(tree)) {
				ImGui::DragFloat2("Scale", &scale.x, 1.0f, 0.0f);
				ImGui::DragFloat("Rotation", &rotation, 0.05f);
				ImGui::DragFloat2("translation", &translation.x, 1.0f);
				ImGui::ColorEdit4("color", &color.x);

				if (selectingTexture_ != 999) {
					if (ImGui::Button("Change to selected texture")) {
						sprites_[i]->SetTextureHandle(selectingTexture_);
					}
				}
				
				if (ImGui::Button("Delete This")) {
					isDeleteSprites_[i] = true;
				}

				ImGui::TreePop();
			}

			sprites_[i]->SetSize(scale);
			sprites_[i]->SetRotation(rotation);
			sprites_[i]->SetPosition(translation);
			sprites_[i]->SetColor(color);

		}

		// 消す
		for (int i = 0; i < kMaxSprites_; i++) {
			if (isDeleteSprites_[i]) {
				std::erase(sprites_, sprites_[i]);
				isDeleteSprites_[i] = false;
			}
		}

		ImGui::TreePop();
	}
#pragma endregion
	#pragma region 球のデバック
	if (ImGui::TreeNode("Spheres")) {
		if (spheres_.size() == 0)
			ImGui::Text("NoSpheres!");

		// 全ての三角形の値変更
		for (int i = 0; i < spheres_.size(); i++) {

			// 三角形の座標取得
			Vector3 scale = spheres_[i]->GetSize();
			Vector3 rotation = spheres_[i]->GetRotation();
			Vector3 translation = spheres_[i]->GetPosition();
			Vector4 color = spheres_[i]->GetColor();

			if (rotation.x >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.x)
				rotation.x = 0.0f;
			if (rotation.y >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.y)
				rotation.y = 0.0f;
			if (rotation.z >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.z)
				rotation.z = 0.0f;

			char tree[16] = "";
			sprintf_s(tree, "Spheres%d", i);
			if (ImGui::TreeNode(tree)) {
				ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.0f);
				ImGui::DragFloat3("Rotation", &rotation.x, 0.05f);
				ImGui::DragFloat3("translation", &translation.x, 0.05f);
				ImGui::ColorEdit4("color", &color.x);

				if (selectingTexture_ != 999) {
					if (ImGui::Button("Change to selected texture")) {
						spheres_[i]->SetTextureHandle(selectingTexture_);
					}
				}

				if (ImGui::Button("Delete This")) {
					isDeletespheres_[i] = true;
				}

				ImGui::TreePop();
			}

			spheres_[i]->SetSize(scale);
			spheres_[i]->SetRotation(rotation);
			spheres_[i]->SetPosition(translation);
			spheres_[i]->SetColor(color);

		}

		// 消す
		for (int i = 0; i < kMaxSpheres_; i++) {
			if (isDeletespheres_[i]) {
				std::erase(spheres_, spheres_[i]);
				isDeletespheres_[i] = false;
			}
		}

		ImGui::TreePop();
	}
#pragma endregion
	#pragma region objのデバック
	if (ImGui::TreeNode("Models")) {
		if (models_.size() == 0)
			ImGui::Text("NoModels!");

		// 全ての三角形の値変更
		for (int i = 0; i < models_.size(); i++) {

			// 三角形の座標取得
			Vector3 scale = models_[i]->GetSize();
			Vector3 rotation = models_[i]->GetRotation();
			Vector3 translation = models_[i]->GetPosition();
			Vector4 color = models_[i]->GetColor();

			if (rotation.x >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.x)
				rotation.x = 0.0f;
			if (rotation.y >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.y)
				rotation.y = 0.0f;
			if (rotation.z >= (float)std::numbers::pi * 2.0f || (float)-std::numbers::pi * 2.0f >= rotation.z)
				rotation.z = 0.0f;

			char tree[16] = "";
			sprintf_s(tree, "Model%d", i);
			if (ImGui::TreeNode(tree)) {
				ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.0f);
				ImGui::DragFloat3("Rotation", &rotation.x, 0.05f);
				ImGui::DragFloat3("translation", &translation.x, 0.05f);
				ImGui::ColorEdit4("color", &color.x);

				if (selectingTexture_ != 999) {
					if (ImGui::Button("Change to selected texture")) {
						models_[i]->SetTextureHandle(selectingTexture_);
					}
				}

				if (ImGui::Button("Delete This")) {
					isDeleteModels_[i] = true;
				}

				ImGui::TreePop();
			}

			models_[i]->SetSize(scale);
			models_[i]->SetRotation(rotation);
			models_[i]->SetPosition(translation);
			models_[i]->SetColor(color);

		}

		// 消す
		for (int i = 0; i < kMaxModels_; i++) {
			if (isDeleteModels_[i]) {
				std::erase(models_, models_[i]);
				isDeleteModels_[i] = false;
			}
		}

		ImGui::TreePop();
	}
#pragma endregion
	ImGui::End();

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 三角形描画前処理
	Triangle::PreDraw(commandList);

	// 全ての三角形の描画
	for (Triangle* triangle : triangles_) {
		triangle->Draw(viewProjectionMatrix_);
	}

	// 三角形描画後処理
	Triangle::PostDraw();

	// 球描画
	Sphere::PreDraw(commandList);

	// 全ての球の描画
	for (Sphere* sphere : spheres_) {
		sphere->Draw(viewProjectionMatrix_);
	}

	Sphere::PostDraw();

	// 3Dモデル描画
	Model::PreDraw(commandList);

	// 全てのモデルの描画
	for (Model* model : models_) {
		model->Draw(viewProjectionMatrix_);
	}

	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 全てのスプライトの描画
	for (Sprite* sprite : sprites_) {
		sprite->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

}