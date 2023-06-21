#pragma once
#include <Windows.h>
#include <cstdlib>
#include <wrl.h>
#include <vector>
#include <chrono>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "WinApp.h"
#include "Debug.h"

/// <summary>
/// DirectX汎用クラス
/// </summary>
class DirectXCommon
{

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="win">ウィンドウズアプリケーションクラス</param>
	/// <param name="backBufferWidth">ウィンドウ横幅</param>
	/// <param name="backBufferHeight">ウィンドウ縦幅</param>
	void Initialize(WinApp* win, 
		int32_t backBufferWidth = WinApp::kWindowWidth, int32_t backBufferHeight = WinApp::kwindowHeight);

private: // メンバ変数

	// ウィンドウズアプリケーションクラス
	WinApp* winApp_;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_; // DXGIファクトリー
	Microsoft::WRL::ComPtr<ID3D12Device> device_;       // デバイス
	int32_t backBufferWidth_ = 0;						// ウィンドウ横幅
	int32_t backBufferHeight_ = 0;						// ウィンドウ縦幅

private: // メンバ関数

	// シングルトンパターンの設定
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	/// <summary>
	/// DXGIデバイスの初期化
	/// </summary>
	void InitializeDXGIDevice();

};

