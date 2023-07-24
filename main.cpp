#include <Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Triangle.h"
#include "GameScene.h"
#include "SafeDelete.h"

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

	// スプライトクラスの静的初期化
	Triangle::StaticInitialize(dxCommon->GetDevice());

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

			// ゲーム固有の処理

			// ゲームシーンの更新処理
			gameScene->Update();

			// 描画開始
			dxCommon->PreDraw();

			// ゲームシーンの描画
			gameScene->Draw();

			// 描画終了
			dxCommon->PostDraw();
		}

	}

	// 解放を行う
	SafeDelete(gameScene);

	// ゲームウィンドウを閉じる
	winApp->TerminateGameWindow();

	// 終了
	return 0;

}