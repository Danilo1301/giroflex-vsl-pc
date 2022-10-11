#pragma once

#include "pch.h"

class PatternLoop {
public:
	std::string Name = "";
	std::vector<int*> Steps;
	int StepIndex = 0;
	int Time = 0;
	int TotalTime = 0;
	bool StepChanged = false;
	bool DontChangeSteps = false;

	void AddStep(int* time) {
		Steps.push_back(time);
	}

	void Clear() {
		Steps.clear();
		Reset();
	}

	bool HasNoSteps()
	{
		return Steps.size() == 0;
	}

	bool HasStepChanged()
	{
		return StepChanged;
	}

	void SetStep(int index)
	{
		Reset();

		StepIndex = index;
		if (StepIndex >= (int)Steps.size())
		{
			StepIndex = 0;
		}
	}

	void Update(int time) {
		StepChanged = false;

		if (Steps.size() == 0) return;

		Time += time;
		TotalTime += time;

		int maxtime = *Steps[StepIndex];

		bool stepChanged = false;

		while (Time >= maxtime)
		{
			Time -= maxtime;

			if (!DontChangeSteps)
			{
				StepIndex++;
				StepChanged = true;

				if (StepIndex >= (int)Steps.size()) {
					StepIndex = 0;
					TotalTime = 0;
				}

				maxtime = *Steps[StepIndex];
			}
		}

		if (StepChanged) {
			Time = 0;
			//Log::file << "[PatternLoop " << name << "] step=" << m_StepIndex << "/" << m_Steps.size() << ", time " << m_Time << "" << *m_Steps[m_StepIndex] << std::endl;
		}

		//Log::file << "[] step=" << m_StepIndex << " steps=" << m_Steps.size() << " time " << m_Time << std::endl;
	}

	void Reset() {
		StepIndex = 0;
		Time = 0;
		TotalTime = 0;
	}
};