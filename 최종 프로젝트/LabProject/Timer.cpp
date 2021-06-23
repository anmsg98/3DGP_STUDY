#include "stdafx.h"
#include "Timer.h"

CGameTimer::CGameTimer()
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequencyPerSec);
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastPerformanceCounter);

	m_fTimeScale = 1.0 / (double)m_nPerformanceFrequencyPerSec;
	m_nBasePerformanceCounter = m_nLastPerformanceCounter;
}

CGameTimer::~CGameTimer()
{
}

void CGameTimer::Tick(float fLockFPS)
{
	if (m_bStopped)
	{
		m_fTimeElapsed = 0.0f;
		return;
	}

	::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);

	float fTimeElapsed = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);

	if (fLockFPS > 0.0f)
	{
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentPerformanceCounter);
			fTimeElapsed = float((m_nCurrentPerformanceCounter - m_nLastPerformanceCounter) * m_fTimeScale);
		}
	}
	
	m_nLastPerformanceCounter = m_nCurrentPerformanceCounter;

	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;

		if (m_nSampleCount < MAX_SAMPLE_COUNT)
		{
			m_nSampleCount++;
		}
	}

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;

	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	m_fTimeElapsed = 0.0f;

	for (ULONG i = 0; i < m_nSampleCount; i++)
	{
		m_fTimeElapsed += m_fFrameTime[i];
	}

	if (m_nSampleCount > 0)
	{
		m_fTimeElapsed /= m_nSampleCount;
	}
}

void CGameTimer::Reset()
{
	__int64 nPerformanceCounter{};

	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nBasePerformanceCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;
	m_bStopped = false;
}

void CGameTimer::Start()
{
	__int64 nPerformanceCounter{};

	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	if (m_bStopped)
	{
		m_nPausedPerformanceCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		m_bStopped = false;
	}
}

void CGameTimer::Stop()
{
	if (!m_bStopped)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nStopPerformanceCounter);
		m_bStopped = true;
	}
}

unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	if (lpszString)
	{
		// 현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력한다.
		// m_pszBuffer 문자열이 "LapProject ("으로 초기화되었으므로 m_pszFrameRate + 12에서부터 프레임 레이트를 문자열로 출력하여 "FPS)" 문자열과 합친다.
		_itow_s(m_nCurrentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return m_nCurrentFrameRate;
}

float CGameTimer::GetTimeElapsed()
{
	return m_fTimeElapsed;
}

float CGameTimer::GetTotalTime()
{
	if (m_bStopped)
	{
		return (float(((m_nStopPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale));
	}

	return (float(((m_nCurrentPerformanceCounter - m_nPausedPerformanceCounter) - m_nBasePerformanceCounter) * m_fTimeScale));
}
