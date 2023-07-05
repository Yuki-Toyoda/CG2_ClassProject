#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

/// <summary>
/// 頂点データ構造体
/// </summary>
struct VertexData {
	Vector4 position;
};

/// <summary>
/// 球構造体
/// </summary>
struct Sphere {
	Vector3 center; // 中心座標
	float radius; // 半径	
};

/// <summary>
/// 平面構造体
/// </summary>
struct Plane {
	Vector3 normal; // 法線
	float distance; // 距離
};

/// <summary>
/// 線構造体
/// </summary>
struct Line {
	Vector3 origin; // 始点
	Vector3 diff; // 終点への差分ベクトル
};

/// <summary>
/// 半直線構造体
/// </summary>
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff; // 終点への差分ベクトル
};

/// <summary>
/// 線分構造体
/// </summary>
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff; // 終点
};

/// <summary>
/// 拡縮、回転、位置の情報を持つ構造体
/// </summary>
struct MyTransform {
	Vector3 scale; // 拡縮
	Vector3 rotate; // 回転
	Vector3 translate; // 位置
};
