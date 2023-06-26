#pragma once

// 3次元ベクトル構造体
struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3 v) const {
		// 結果格納用
		Vector3 result;

		// 計算処理
		result.x = x + v.x;
		result.y = y + v.y;
		result.z = z + v.z;

		// 計算結果の返却
		return result;
	}

	Vector3 operator-(const Vector3 v) const {
		// 結果格納用
		Vector3 result;

		// 計算処理
		result.x = x - v.x;
		result.y = y - v.y;
		result.z = z - v.z;

		// 計算結果の返却
		return result;
	}

	Vector3 operator*(const Vector3 v) const {
		// 結果格納用
		Vector3 result;

		// 計算処理
		result.x = x * v.x;
		result.y = y * v.y;
		result.z = z * v.z;

		// 計算結果の返却
		return result;
	}

};
