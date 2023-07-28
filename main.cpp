#include <Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Triangle.h"
#include "Sprite.h"
#include "Sphere.h"
#include "GameScene.h"
#include "TextureManager.h"
#include "SafeDelete.h"
#include "ImGuiManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// 空のインスタンスを生成
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

	// ゲームシーン生成
	GameScene* gameScene = nullptr;

	// ウィンドウズアプリケーションクラスのインスタンスを取得
	winApp = WinApp::GetInstance();
	// ゲームウィンドウを生成する
	winApp->CreateGameWindow();

	// DirectX汎用クラスのインスタンスを取得
	dxCommon = DirectXCommon::GetInstance();
	// DirectXの初期化
	dxCommon->Initialize(winApp);

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetImstance();
	imguiManager->Intialize(winApp, dxCommon);

	// テクスチャマネージャ初期化
	TextureManager::GetInstance()->Intialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// 三角形クラスの静的初期化
	Triangle::StaticInitialize(dxCommon->GetDevice());
	// スプライトクラスの静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice());
	// 球クラスの静的初期化
	Sphere::StaticInitialize(dxCommon->GetDevice());

	// ゲームシーン初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {

		// ゲームが終了するメッセージを受けとったら
		if (winApp->ProcessMessage()) {
			// ゲーム終了
			break;
		}
		else {

			// ImGui受付開始
			imguiManager->Begin();

			// ゲーム固有の処理
			// ゲームシーンの更新処理
			gameScene->Update();

			// ImGui受付終了
			imguiManager->End();

			// 描画開始
			dxCommon->PreDraw();

			// ゲームシーンの描画
			gameScene->Draw();

			// ImGui描画
			imguiManager->Draw();

			// 描画終了
			dxCommon->PostDraw();
		}

	}

	// 解放を行う
	SafeDelete(gameScene);

	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウを閉じる
	winApp->TerminateGameWindow();

	// 終了
	return 0;

}