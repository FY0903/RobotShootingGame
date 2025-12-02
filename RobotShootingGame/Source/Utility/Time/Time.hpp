/*+===================================================================
	File: Time.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/02 15:26:16 初回作成
	（これ以降下に更新日時と更新内容を書く）
===================================================================+*/
#pragma once

/**
 * @brief Timeクラス
 */
class Time : public Singleton<Time>
{
public:
	void Init(float timeScale = 1.0f, float fixedDeltaTime = 0.02f);

	void Update();

	inline float GetDeltaTime() const { return m_deltaTime; }
	inline float GetTime() const { return m_time; }
	inline float GetDeltaTimeSinceStartup() const { return m_deltaTimeSinceStartup; }
	inline float GetTimeSinceStartup() const { return m_timeSinceStartup; }
	uint64_t GetFrameCount() const { return m_frameCount; }

	inline float GetTimeScale() const { return m_timeScale; }
	inline float GetFixedDeltaTime() const { return m_fixedDeltaTime; }

	void SetTimeScale(float timeScale);
	void SetFixedDeltaTime(float fixedDeltaTime);

	int ConsumeFixedUpdateSteps(int maxPerFrame = 5);

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

	std::chrono::steady_clock::time_point m_lastTime{};
	std::chrono::steady_clock::time_point m_NowTime{};

	float m_deltaTime{};
	float m_time{};
	float m_deltaTimeSinceStartup{};
	float m_timeSinceStartup{};
	float m_timeScale{};

	// 固定更新関連
	float m_fixedDeltaTime{};
	float m_fixedAccumulator{};

	// フレームカウント
	uint64_t m_frameCount{};
};
