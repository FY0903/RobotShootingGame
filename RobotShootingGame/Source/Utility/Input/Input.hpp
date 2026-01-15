/*+===================================================================
	File: Input.hpp
	Summary: Inputクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/09/04 15:04 初回作成
			26/01/15 18:44 コメント記載
===================================================================+*/
#pragma once

#undef max	// minマクロの影響を防ぐために#undef
#undef min	// maxマクロの影響を防ぐために#undef

namespace Input
{
	/**
	 * @brief 初期化処理
	 * @return HRESULT 初期化の成否を示すコード。
	 */
	HRESULT Init();

	/**
	 * @brief 更新処理
	 */
	void Update();

	/**
	 * @brief マウスのウィンドウ上の相対座標を更新します。
	 * @param In_Lparam マウスの相対座標を含む LPARAM 値。
	 */
	void UpdateRelativeMousePos(LPARAM In_Lparam);

	/**
	 * @brief マウスホイールの入力情報を更新します。
	 * @param wheelDelta マウスホイールの回転量を示すショート整数値。
	 */
	void UpdateMouseWheel(short wheelDelta);

	/**
	 * @brief マウスの中ボタンの入力情報を更新します。
	 * @param pressed 中ボタンが押されている場合は true、そうでない場合は false。
	 */
	void UpdateMiddleButton(bool pressed);

	/**
	 * @brief フレーム終了時の入力情報の更新処理
	 */
	void EndUpdateInput();

	/**
	 * @brief 指定されたキーが押されているかどうかを判定します。
	 * @param key 判定するキーのコード。
	 * @return true キーが押されている場合。
	 * @return false キーが押されていない場合。
	 */
	bool IsKeyPress(BYTE key);

	/**
	 * @brief 指定されたキーがトリガーされたかどうかを判定します。
	 * @param key 判定するキーのコード。
	 * @return true キーがトリガーされた場合。
	 * @return false キーがトリガーされていない場合。
	 */
	bool IsKeyTrigger(BYTE key);

	/**
	 * @brief 指定されたキーがリリースされたかどうかを判定します。
	 * @param key 判定するキーのコード。
	 * @return true キーがリリースされた場合。
	 * @return false キーがリリースされていない場合。
	 */
	bool IsKeyRelease(BYTE key);

	/**
	 * @brief 指定されたキーがリピートされているかどうかを判定します。
	 * @param key 判定するキーのコード。
	 * @return true キーがリピートされている場合。
	 * @return false キーがリピートされていない場合。
	 */
	bool IsKeyRepeat(BYTE key);

	/**
	 * @brief 任意のキーが押されているかどうかを判定します。
	 * @return true 任意のキーが押されている場合。
	 * @return false すべてのキーが押されていない場合。
	 */
	bool IsAnyKeyPress();

	/**
	 * @brief マウスの現在の座標を取得します。
	 * @return const POINT& マウスの現在の座標を表す POINT 構造体への定数参照。
	 */
	const POINT& GetMousePos();

	/**
	 * @brief マウスの前回の座標を取得します。
	 * @return const POINT& マウスの前回の座標を表す POINT 構造体への定数参照。
	 */
	POINT GetMouseDifferencePos();

	/**
	 * @brief マウスのウィンドウ上の相対座標を取得します。
	 * @return POINTS マウスのウィンドウ上の相対座標を表す POINTS 構造体。
	 */
	POINTS GetMouseRelativePos();

	/**
	 * @brief マウスホイールの入力情報を取得します。
	 * @return int マウスホイールの回転量を示す整数値。
	 */
	int GetMouseWheelInput();

	/**
	 * @brief マウスの中ボタンが押されているかどうかを判定します。
	 * @return true マウスの中ボタンが押されている場合。
	 * @return false マウスの中ボタンが押されていない場合。
	 */
	bool IsMiddleButtonPress();
}
