/*+===================================================================
	File: Time.cpp
	Summary: Timeクラス実装
	Author: AT13C192 23 藤原佑埜
	Date: 2025/12/02 15:26 初回作成
            26/01/16 11:48 コメント記載
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Time.hpp"

// ==============================
//	define
// ==============================
#undef min

// ==============================
//	constexpr
// ==============================
constexpr float MAX_DELTA_TIME = 0.25f; // フレーム落ち対策での最大デルタタイム

void Time::Init(float timeScale, float fixedDeltaTime)
{
	m_NowTime = m_lastTime = std::chrono::steady_clock::now();
	SetTimeScale(timeScale);
	SetFixedDeltaTime(fixedDeltaTime);
	m_maxPerFrame = static_cast<int>(MAX_DELTA_TIME / m_fixedDeltaTime);	// 1フレームあたりの最大固定更新回数を計算
}

void Time::Update()
{
	m_NowTime = std::chrono::steady_clock::now();

	const float delta = std::chrono::duration<float>(m_NowTime - m_lastTime).count();
	const float clampedDelta = std::clamp(delta, 0.0f, MAX_DELTA_TIME);

	// 更新時間の計算
	m_time = clampedDelta;
	m_deltaTime = clampedDelta * m_timeScale;

	// 累積時間の更新
	m_timeSinceStartup += m_time;
	m_deltaTimeSinceStartup += m_deltaTime;

	// 固定更新用アキュムレータの更新
	m_fixedAccumulator += m_time;

	m_lastTime = m_NowTime;
	++m_frameCount;
}

void Time::SetTimeScale(float timeScale)
{
	// 不正な値の場合はデフォルト値に設定
	if (std::isnan(timeScale) || timeScale < 0.0f) timeScale = 1.0f;
	m_timeScale = timeScale;
}

void Time::SetFixedDeltaTime(float fixedDeltaTime)
{
	// 不正な値の場合はデフォルト値に設定
	if (std::isnan(fixedDeltaTime) || fixedDeltaTime <= 0.0f) fixedDeltaTime = 0.02f;
	m_fixedDeltaTime = fixedDeltaTime;
}

int Time::ConsumeFixedUpdateSteps()
{
	// 固定更新ステップ数を計算
	int steps = static_cast<int>(m_fixedAccumulator / m_fixedDeltaTime);

	// 最大値を超える場合は最大値に制限
	steps = std::min(steps, m_maxPerFrame);

	if (steps > 0)
	{
		// アキュムレータを減算
		m_fixedAccumulator -= static_cast<float>(steps) * m_fixedDeltaTime;
		if (m_fixedAccumulator < 0.0f) m_fixedAccumulator = 0.0f;	// 負の値にならないように補正
	}
	return steps;
}
