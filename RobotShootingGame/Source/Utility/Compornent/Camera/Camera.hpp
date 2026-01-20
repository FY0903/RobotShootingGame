/*+===================================================================
	File: Camera.hpp
	Summary: Cameraクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/11/20 18:19 初回作成
			26/01/15 17:03 コメント記載
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Utility/Compornent/Component.hpp"
#include "System/main.hpp"
#include "System/Engine/Engine.hpp"
#include "Utility/ConstantBuffer/ConstantBuffer.hpp"
#include "Utility/RootSignature/RootSignature.hpp"

/**
 * @brief Cameraクラス
 */
class Camera : public Component
{
public:
	Component::Component;

	/**
	 * コンストラクタ
	 */
	Camera() = default;

	/**
	 * デストラクタ
	 */
	~Camera() = default;

	/**
	 * @brief カメラ（視点）パラメータを初期化します。視点位置、注視点、上方向、半径、視野角、アスペクト比を設定します。
	 * @param eyePos 視点位置を表す DirectX::XMVECTOR（ワールド空間のカメラ位置）。
	 * @param targetPos 注視点を表す DirectX::XMVECTOR（カメラが向く座標）。
	 * @param upVec 上方向ベクトルを表す DirectX::XMVECTOR（カメラの上方向）。
	 * @param radius カメラの半径または視点と注視点間の距離（float）。
	 * @param fov 垂直視野角（ラジアン、float）。
	 * @param aspect アスペクト比（幅 / 高さ、float）。
	 */
	void Init(DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 5.0f, 5.0f, 0.0f),
		DirectX::XMVECTOR targetPos = DirectX::XMVectorZero(),
		DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		float radius = 10.0f,
		float fov = DirectX::XMConvertToRadians(60.0f),
		float aspect = 16.0f / 9.0f);

	/**
	 * @brief 更新処理
	 */
	void Update() override final;

	/**
	 * @brief 描画処理
	 */
	void Draw() override final {}

	/**
	 * @brief 終了処理
	 */
	void Uninit() override final {}

	/**
	 * @brief ターゲット位置を設定します。内部の m_TargetPos メンバーに値を代入します。
	 * @param targetPos 設定するターゲット位置。DirectX::XMVECTOR 型のベクトル。
	 */
	inline void SetTargetPos(DirectX::XMVECTOR targetPos) { m_TargetPos = targetPos; }

	/**
	 * @brief メンバー変数 m_Radius に半径を設定します。
	 * @param radius 設定する新しい半径の値（float）。
	 */
	inline void SetRadius(float radius) { m_Radius = radius; }

	/**
	 * @brief DirectX::XMFLOAT4X4 型の3Dビュー行列を取得します。
	 * @param transpose 行列を転置して返すかどうか。既定値は true（転置する）。
	 * @return DirectX::XMFLOAT4X4 型の3Dビュー行列を値として返します。transpose が true の場合は転置された行列を返します。
	 */
	const DirectX::XMFLOAT4X4 Get3DViewMatrixFloat4x4(bool transpose = true);

	/**
	 * @brief DirectX::XMFLOAT4X4 型の3Dプロジェクション行列を取得します。
	 * @param transpose 行列を転置して返すかどうか。既定値は true（転置する）。
	 * @return DirectX::XMFLOAT4X4 型の3Dプロジェクション行列を値として返します。transpose が true の場合は転置された行列を返します。
	 */
	const DirectX::XMFLOAT4X4 Get3DProjectionMatrixFloat4x4(bool transpose = true);

	/**
	 * @brief DirectX::XMFLOAT4X4 型の2Dビュー行列を取得します。
	 * @param transpose 行列を転置して返すかどうか。既定値は true（転置する）。
	 * @return DirectX::XMFLOAT4X4 型の2Dビュー行列を値として返します。transpose が true の場合は転置された行列を返します。
	 */
	const DirectX::XMFLOAT4X4 Get2DViewMatrixFloat4x4(bool transpose = true);

	/**
	 * @brief DirectX::XMFLOAT4X4 型の2Dプロジェクション行列を取得します。
	 * @param transpose 行列を転置して返すかどうか。既定値は true（転置する）。
	 * @return DirectX::XMFLOAT4X4 型の2Dプロジェクション行列を値として返します。transpose が true の場合は転置された行列を返します。
	 */
	const DirectX::XMFLOAT4X4 Get2DProjectionMatrixFloat4x4(bool transpose = true);

	/**
	 * @brief オブジェクトが保持する 3D ビュー行列への const 参照を返します。
	 * @return 3D ビュー行列への const 参照。
	 */
	const DirectX::XMMATRIX& Get3DViewMatrix() const { return m_3DVP[0]; }

	/**
	 * @brief オブジェクトが保持する 3D プロジェクション行列への const 参照を返します。
	 * @return 3D プロジェクション行列への const 参照。
	 */
	const DirectX::XMMATRIX& Get3DProjectionMatrix() const { return m_3DVP[1]; }
	
	/**
	 * @brief オブジェクトが保持する 2D ビュー行列への const 参照を返します。
	 * @return 2D ビュー行列への const 参照。
	 */
	const DirectX::XMMATRIX& Get2DViewMatrix() const { return m_2DVP[0]; }
	
	/**
	 * @brief オブジェクトが保持する 2D プロジェクション行列への const 参照を返します。
	 * @return 2D プロジェクション行列への const 参照。
	 */
	const DirectX::XMMATRIX& Get2DProjectionMatrix() const { return m_2DVP[1]; }

private:
	DirectX::XMMATRIX m_3DVP[2]{};	// 3Dビュー行列とプロジェクション行列
	DirectX::XMMATRIX m_2DVP[2]{};	// 2Dビュー行列とプロジェクション行列

	DirectX::XMVECTOR m_EyePos{};		// 視線位置
	DirectX::XMVECTOR m_TargetPos{};	// 注視点
	DirectX::XMVECTOR m_UpVec{};		// 上方ベクトル

	float m_Radius{};	// 半径
	float m_RadXZ{};	// XZ平面でのラジアン
	float m_RadY{};		// Y軸でのラジアン

	float m_Fov{};		// 視野角
	float m_Aspect{};	// アスペクト比
};
