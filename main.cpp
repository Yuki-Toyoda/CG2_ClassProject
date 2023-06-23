#include <Windows.h>
#include "WinApp.h"
#include "DirectXCommon.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// 空のインスタンスを生成
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

	// ウィンドウズアプリケーションクラスのインスタンスを取得
	winApp = WinApp::GetInstance();
	// ゲームウィンドウを生成する
	winApp->CreateGameWindow();

	// DirectX汎用クラスのインスタンスを取得
	dxCommon = DirectXCommon::GetInstance();
	// DirectXの初期化
	dxCommon->Initialize(winApp);

	// メインループ
	while (true) {

		// ゲームが終了するメッセージを受けとったら
		if (winApp->ProcessMessage()) {
			// ゲーム終了
			break;
		}
		else {

			// ゲーム固有の処理

			// 描画開始
			dxCommon->PreDraw();

			// 描画終了
			dxCommon->PostDraw();
		}

	}

	// 終了したら生成したオブジェクトの解放を行う
	winApp->TerminateGameWindow();

	// 終了
	return 0;

}