#include "TestHelper.h"
#include "CFont.h"

std::vector<TestFunction*> TestHelper::m_Steps;

void TestHelper::AddStep(std::string name, std::function<void(void)> fn)
{
	TestFunction* testFunction = new TestFunction{ name, fn };
	m_Steps.push_back(testFunction);
}

void TestHelper::RemoveStep(TestFunction* step)
{
	auto it = std::find(m_Steps.begin(), m_Steps.end(), step);
	if (it == m_Steps.end()) return;
	m_Steps.erase(it);
	delete step;
}

void TestHelper::RunNextStep()
{
	if (m_Steps.size() == 0) return;

	auto testFunction = m_Steps[0];

	auto fn = testFunction->fn;

	static char message[256];
	snprintf(message, 256, "[TestHelper] %s", testFunction->name.c_str());
	CMessages::AddMessageJumpQ(message, 2000, 0, false);

	RemoveStep(testFunction);

	fn();
}

void TestHelper::RunAllSteps()
{
	while (m_Steps.size() > 0)
	{
		RunNextStep();
	}
}


std::vector<std::string> TestHelper::m_DebugLines;

void TestHelper::Draw()
{
	float x = 30, y = 30;

	for (auto line : m_DebugLines)
	{
		char buffer[256];
		sprintf_s(buffer, "%s", line.c_str());

		CFont::m_fWrapx = 100000.0f;
		CFont::SetOrientation(ALIGN_LEFT);
		CFont::SetColor(CRGBA(255, 255, 0));
		CFont::SetDropShadowPosition(1);
		CFont::SetBackground(false, false);
		CFont::SetWrapx(500.0);
		CFont::SetScale(0.5 * 0.7f, 1.0 * 0.7f);
		CFont::SetFontStyle(FONT_SUBTITLES);
		CFont::SetProportional(true);

		CFont::PrintString(x, y, buffer);

		y += 20;
	}
}

void TestHelper::AddLine(std::string text)
{
	m_DebugLines.push_back(text);

	if (m_DebugLines.size() >= 30) m_DebugLines.erase(m_DebugLines.begin());
}