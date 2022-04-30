#include "TestHelper.h"

std::vector<TestFunction*> TestHelper::m_Steps;

void TestHelper::AddStep(std::string name, std::function<void(void)> fn) {
	TestFunction* testFunction = new TestFunction{ name, fn };
	m_Steps.push_back(testFunction);
}

void TestHelper::RemoveStep(TestFunction* step) {
	auto it = std::find(m_Steps.begin(), m_Steps.end(), step);
	if (it == m_Steps.end()) return;
	m_Steps.erase(it);
	delete step;
}

void TestHelper::RunNextStep() {
	if (m_Steps.size() == 0) return;

	auto testFunction = m_Steps[0];

	auto fn = testFunction->fn;

	static char message[256];
	snprintf(message, 256, "[TestHelper] %s", testFunction->name.c_str());
	CMessages::AddMessageJumpQ(message, 2000, 0, false);

	RemoveStep(testFunction);

	fn();
}


void TestHelper::RunAllSteps() {
	while (m_Steps.size() > 0)
	{
		RunNextStep();
	}
}