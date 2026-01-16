/*+===================================================================
	File: Transform.hpp
	Summary: Transform構造体ヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/14 11:54 初回作成
            26/01/16 11:51 コメント記載
===================================================================+*/

#pragma once

struct Transform
{
	DirectX::SimpleMath::Vector3 Position{};	// 位置
	DirectX::SimpleMath::Quaternion Rotation{}; // 回転（オイラー角）
	DirectX::SimpleMath::Vector3 Scale{};		// スケール

	/**
	 * @brief コンストラクタ
	 */
	Transform()
		: Position(0.0f, 0.0f, 0.0f)
		, Rotation(0.0f, 0.0f, 0.0f, 1.0f)
		, Scale(1.0f, 1.0f, 1.0f)
	{
	}

	/**
	 * @brief 度単位の角度をラジアンに変換する関数。
	 * @param degree 変換する角度（度単位）。
	 * @return ラジアン単位で表した角度（float）。
	 */
	static inline float ToRadian(float degree)
	{
		return degree * (DirectX::XM_PI / 180.0f);
	}

	/**
	 * @brief オイラー角から対応する回転を表すDirectX::SimpleMath::Quaternionを生成します。
	 * @param eulerAngles yaw, pitch, roll を格納した DirectX::SimpleMath::Vector3 への参照。各成分の単位は CreateFromYawPitchRoll が期待するもの（通常はラジアン）に従います。
	 * @return 指定したオイラー角を表す DirectX::SimpleMath::Quaternion。
	 */
	static inline DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& eulerAngles)
	{
		// TODO: 後で自分で作成する
		return DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerAngles);
	}

	/**
	 * @brief 四元数をオイラー角（Vector3）に変換します。
	 * @param quaternion 変換対象の四元数。const 参照で渡されます。
	 * @return DirectX::SimpleMath::Vector3 型で表されたオイラー角（ピッチ・ヨー・ロールを表す3要素）を返します。
	 */
	static inline DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& quaternion)
	{
		// TODO: 後で自分で作成する
		return quaternion.ToEuler();
	}

	/**
	 * @brief スケール、回転（四元数）、並進を合成してワールド変換行列を生成して返します。スケール→回転→並進の順に適用されます。
	 * @return DirectX::XMMATRIX: スケール、回転、並進を適用したワールド変換行列を返します。
	 */
	inline DirectX::XMMATRIX GetWorldMatrix()
	{
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(Rotation);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z);
		return scaleMatrix * rotationMatrix * translationMatrix;
	}

	/**
	 * @brief DirectX::XMFLOAT4X4 型でワールド行列を取得します。必要に応じて行列を転置して返します。
	 * @param transpose 行列を転置するかどうかを指定します。
	 * @return DirectX::XMFLOAT4X4 型のワールド行列。transpose が true の場合は転置済みの行列を返します。
	 */
	inline DirectX::XMFLOAT4X4 GetWorldMatrixFloat4x4(bool transpose = true)
	{
		DirectX::XMFLOAT4X4 worldf{};
		DirectX::XMMATRIX worldMat = GetWorldMatrix();

		if (transpose)
			worldMat = DirectX::XMMatrixTranspose(worldMat);

		DirectX::XMStoreFloat4x4(&worldf, worldMat);

		return worldf;
	}
};
