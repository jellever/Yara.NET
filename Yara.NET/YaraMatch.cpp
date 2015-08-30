#include "stdafx.h"


namespace YaraNET
{
	YaraMatch::YaraMatch(YaraRule^ rule)
	{
		this->matchData = gcnew List<YaraString^>();
		this->rule = rule;
	}

	void YaraMatch::AddMatch(YaraString^ yaraString)
	{
		this->matchData->Add(yaraString);
	}
}