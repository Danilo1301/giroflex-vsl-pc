#pragma once

#include "pch.h"
#include "Pattern.h"

class Patterns {
public:
	static std::vector<Pattern*> m_Patterns;

	static Pattern* CreatePattern(std::string name);
	static void RemovePattern(Pattern* pattern);
	static void RemoveAllPatterns();
};