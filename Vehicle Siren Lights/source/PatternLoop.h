#pragma once

#include "pch.h"

class PatternLoop {
private:
	std::vector<int*> m_Steps;
public:
	std::string name = "";

	int m_Time = 0;
	int m_TotalTime = 0;
	int m_StepIndex = 0;

	void AddStep(int* time) {
		m_Steps.push_back(time);
	}

	void Clear() {
		m_Steps.clear();
	}

	std::vector<int*> GetSteps() {
		return m_Steps;
	}

	void Update(int time) {
		if (m_Steps.size() == 0) return;

		m_Time += time;
		m_TotalTime += time;

		int maxtime = *m_Steps[m_StepIndex];

		bool stepChanged = false;

		while (m_Time >= maxtime)
		{
			m_Time -= maxtime;
			m_StepIndex++;

			stepChanged = true;

			if (m_StepIndex >= (int)m_Steps.size()) {
				m_StepIndex = 0;
				m_TotalTime = 0;
			}

			maxtime = *m_Steps[m_StepIndex];
		}

		if (stepChanged) {
			//Log::file << "[PatternLoop " << name << "] step=" << m_StepIndex << "/" << m_Steps.size() << ", time " << m_Time << "" << *m_Steps[m_StepIndex] << std::endl;
		}

		//Log::file << "[] step=" << m_StepIndex << " steps=" << m_Steps.size() << " time " << m_Time << std::endl;
	}

	void Reset() {
		m_StepIndex = 0;
		m_Time = 0;
		m_TotalTime = 0;
	}
};