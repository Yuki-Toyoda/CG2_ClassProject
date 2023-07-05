#pragma once
#include <cassert>
#include <cmath>
#include <numbers>
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "MyStruct.h"

/// <summary>
/// 数学系関数を管理するクラス
/// </summary>
class MyMath
{
public:

#pragma region float系演算関数

		/// <summary>
		/// 内積を求める
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>内積</returns>
		static float Dot(const Vector3& v1, const Vector3& v2);


		/// <summary>
		/// ベクトルの長さを求める
		/// </summary>
		/// <param name="v">ベクトル</param>
		/// <returns>長さ</returns>
		static float Length(const Vector3& v);

		/// <summary>
		/// 値を min から max の値に収める関数
		/// </summary>
		/// <param name="a">収める値</param>
		/// <param name="min">最小値</param>
		/// <param name="max">最大値</param>
		/// <returns>範囲内の値</returns>
		static float Clamp(float a, float min, float max);

#pragma endregion

#pragma region Vector3系演算関数

		/// <summary>
		/// 三次元ベクトルの加算関数
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>加算されたベクトル</returns>
		static Vector3 Add(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// ベクトルの減算
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>減算されたベクトル</returns>
		static Vector3 Subtract(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// ベクトルをスカラー倍する
		/// </summary>
		/// <param name="scalar">何倍</param>
		/// <param name="v">ベクトル</param>
		/// <returns>スカラー倍されたベクトル</returns>
		static Vector3 Multiply(float scalar, const Vector3& v);

		/// <summary>
		/// ベクトルの正規化
		/// </summary>
		/// <param name="v">ベクトル</param>
		/// <returns>正規化されたベクトル</returns>
		static Vector3 Normalize(const Vector3& v);

		/// <summary>
		/// 行列のクロス積を求める関数
		/// </summary>
		/// <param name="v1">ベクトル1</param>
		/// <param name="v2">ベクトル2</param>
		/// <returns>クロス積</returns>
		static Vector3 Cross(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// 行列を3次元ベクトルに変換する関数
		/// </summary>
		/// <param name="vector">3次元ベクトル</param>
		/// <param name="matrix">行列</param>
		/// <returns></returns>
		static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

		/// <summary>
		/// 正射影ベクトルを求める関数
		/// </summary>
		/// <param name="v1">点1</param>
		/// <param name="v2">点2</param>
		/// <returns>正射影ベクトル</returns>
		static Vector3 Project(const Vector3& v1, const Vector3& v2);

		/// <summary>
		/// 最近頂点を求める関数
		/// </summary>
		/// <param name="point">点</param>
		/// <param name="segment">線分</param>
		/// <returns>最近頂点</returns>
		static Vector3 ClosestProject(const Vector3& point, const Segment& segment);

		/// <summary>
		/// 法線と垂直なベクトルを求める関数
		/// </summary>
		/// <param name="vector">ベクトル</param>
		/// <returns>垂直なベクトル</returns>
		static Vector3 perpendiculer(const Vector3& vector);

#pragma endregion

#pragma region Matrix4x4系演算関数

		/// <summary>
		/// 行列の乗算
		/// </summary>
		/// <param name="m1">乗算する行列1</param>
		/// <param name="m2">乗算する行列2</param>
		/// <returns></returns>
		static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

		/// <summary>
		/// 逆行列
		/// </summary>
		/// <param name="m">計算する行列</param>
		/// <returns></returns>
		static Matrix4x4 Inverse(const Matrix4x4& m);

		/// <summary>
		/// 平行移動行列
		/// </summary>
		/// <param name="translate">三次元ベクトル</param>
		/// <returns>平行移動した後の行列</returns>
		static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

		/// <summary>
		/// 拡大縮小行列
		/// </summary>
		/// <param name="scale">三次元ベクトル</param>
		/// <returns></returns>
		static Matrix4x4 MakeScaleMatrix(const Vector3& scale);

		/// <summary>
		/// x軸方向の回転行列を作成する関数
		/// </summary>
		/// <param name="radian">回転角度</param>
		/// <returns>回転後の行列</returns>
		static Matrix4x4 MakeRotateXMatrix(float radian);

		/// <summary>
		/// y軸方向の回転行列を作成する関数
		/// </summary>
		/// <param name="radian">回転角度</param>
		/// <returns>回転後の行列</returns>
		static Matrix4x4 MakeRotateYMatrix(float radian);

		/// <summary>
		/// z軸方向の回転行列を作成する関数
		/// </summary>
		/// <param name="radian">回転角度</param>
		/// <returns>回転後の行列</returns>
		static Matrix4x4 MakeRotateZMatrix(float radian);

		/// <summary>
		/// 全ての軸の回転行列を作成する関数
		/// </summary>
		/// <param name="rotate">回転角</param>
		/// <returns>全ての軸の回転行列</returns>
		static Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);

		/// <summary>
		/// 単位行列を作成する関数
		/// </summary>
		/// <returns></returns>
		static Matrix4x4 MakeIdentity4x4();

		/// <summary>
		/// アフィン変換行列を生成する関数
		/// </summary>
		/// <param name="scale">拡大縮小行列</param>
		/// <param name="rotate">回転行列</param>
		/// <param name="translate">平行移動行列</param>
		/// <returns>アフィン変換された行列</returns>
		static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

		/// <summary>
		/// 正射影行列作成関数
		/// </summary>
		/// <param name="left"></param>
		/// <param name="top"></param>
		/// <param name="right"></param>
		/// <param name="bottom"></param>
		/// <param name="nearClip"></param>
		/// <param name="farClip"></param>
		/// <returns>正射影行列</returns>
		static Matrix4x4 MakeOrthGraphicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

		/// <summary>
		/// 透視射影行列作成関数
		/// </summary>
		/// <param name="fovY">画角</param>
		/// <param name="aspectRatio">アスペクト比</param>
		/// <param name="nearClip">近平面への距離</param>
		/// <param name="farClip">遠平面への距離</param>
		/// <returns>透視射影行列</returns>
		static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

		/// <summary>
		/// ビューポート変換行列
		/// </summary>
		/// <param name="left"></param>
		/// <param name="top"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="minDepth"></param>
		/// <param name="maxDepth"></param>
		/// <returns>ビューポート行列</returns>
		static Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

#pragma endregion

};

