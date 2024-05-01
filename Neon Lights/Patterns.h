#pragma once

#include "pch.h"

#include "Pattern.h"

class Patterns {
private:
	static std::vector<Pattern*> m_Patterns;
public:
	static Pattern* CreatePattern(std::string name);
	static Pattern* GetPattern(std::string name);
	static void RemovePattern(Pattern* pattern);
	static void RemoveAllPatterns();
	static std::vector<Pattern*> GetPatterns();

	static int FindPatternIndex(Pattern* pattern)
	{
		return (std::find(m_Patterns.begin(), m_Patterns.end(), pattern) - m_Patterns.begin());
	}

	static Pattern* FindPatternByIndex(int index)
	{
		if (index >= 0 && index <= (int)m_Patterns.size() - 1) return m_Patterns[index];
		return NULL;
	}
};