/*+===================================================================
	File: Time.hpp
	Summary: Timeクラスのヘッダーファイル
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/02 15:26 初回作成
            26/01/16 11:48 コメント記載
===================================================================+*/
#pragma once

/**
 * @brief Timeクラス
 */
class Time : public Singleton<Time>
{
public:
	/**
	 * @brief 時間設定を初期化します。時間スケールと固定更新間隔を設定します。
	 * @param timeScale 時間の進行速度を調整する倍率。
	 * @param fixedDeltaTime 固定更新（例: 物理更新）ごとの時間間隔（秒）。
	 */
	void Init(float timeScale = 1.0f, float fixedDeltaTime = 1.0f / 60.0f);

	/**
	 * @brief 更新処理
	 */
	void Update();

	/**
	 * @brief 前フレームからの経過時間を取得します。
	 * @return 前フレームからの経過時間（秒）。
	 */
	inline float GetDeltaTime() const { return m_deltaTime; }
	
	/**
	 * @brief ゲーム開始からの累積時間を取得します。
	 * @return ゲーム開始からの累積時間（秒）。
	 */
	inline float GetTime() const { return m_time; }
	
	/**
	 * @brief ゲーム開始からの累積時間（時間スケール適用後）を取得します。
	 * @return ゲーム開始からの累積時間（秒、時間スケール適用後）。
	 */
	inline float GetDeltaTimeSinceStartup() const { return m_deltaTimeSinceStartup; }
	
	/**
	 * @brief ゲーム開始からの累積時間を取得します。
	 * @return ゲーム開始からの累積時間（秒）。
	 */
	inline float GetTimeSinceStartup() const { return m_timeSinceStartup; }
	
	/**
	 * @brief フレームカウントを取得します。
	 * @return フレームカウント。
	 */
	uint64_t GetFrameCount() const { return m_frameCount; }

	/**
	 * @brief 時間スケールを取得します。
	 * @return 時間スケール。
	 */
	inline float GetTimeScale() const { return m_timeScale; }
	
	/**
	 * @brief 固定更新間隔を取得します。
	 * @return 固定更新間隔（秒）。
	 */
	inline float GetFixedDeltaTime() const { return m_fixedDeltaTime; }

	/**
	 * @brief 時間スケールを設定します。
	 * @param timeScale 設定する時間スケール。
	 */
	void SetTimeScale(float timeScale);
	
	/**
	 * @brief 固定更新間隔を設定します。
	 * @param fixedDeltaTime 設定する固定更新間隔（秒）。
	 */
	void SetFixedDeltaTime(float fixedDeltaTime);

	/**
	 * @brief 固定更新ステップを消費します。
	 * @return 消費した固定更新ステップの数。
	 */
	int ConsumeFixedUpdateSteps();

private:
	friend class Singleton<Time>;

	/**
	 * コンストラクタ
	 */
	Time() = default;

	/**
	 * デストラクタ
	 */
	~Time() = default;

	std::chrono::steady_clock::time_point m_lastTime{};	// 前フレームの時間
	std::chrono::steady_clock::time_point m_NowTime{};	// 現在の時間

	float m_deltaTime{};				// 前フレームからの経過時間（時間スケール適用後）
	float m_time{};						// 前フレームからの経過時間
	float m_deltaTimeSinceStartup{};	// ゲーム開始からの累積時間（時間スケール適用後）
	float m_timeSinceStartup{};			// ゲーム開始からの累積時間
	float m_timeScale{};				// 時間スケール

	float m_fixedDeltaTime{};			// 固定更新間隔
	float m_fixedAccumulator{};			// 固定更新用アキュムレータ

	int m_maxPerFrame{};	// 1フレームあたりの最大固定更新回数

	uint64_t m_frameCount{};	// フレームカウント
};
