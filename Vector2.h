#pragma once

// 2次元ベクトル
struct Vector2 final {
	float x;
	float y;

	Vector2 operator+(const Vector2 v) {
		// 結果格納用
		Vector2 result;

		// 計算処理
		result.x = x + v.x;
		result.y = y + v.y;

		// 計算結果を返す
		return result;
	}

	Vector2 operator-(const Vector2 v) {
		// 結果格納用
		Vector2 result;

		// 計算処理
		result.x = x - v.x;
		result.y = y - v.y;

		// 計算結果を返す
		return result;
	}

	Vector2 operator*(const Vector2 v) {
		// 結果格納用
		Vector2 result;

		// 計算処理
		result.x = x * v.x;
		result.y = y * v.y;

		// 計算結果を返す
		return result;
	}

};