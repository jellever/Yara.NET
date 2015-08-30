#include "stdafx.h"


namespace YaraNET
{
	YaraMatch::YaraMatch(YaraRule^ rule)
	{
		this->matchData = gcnew List<YaraString^>();
	}

	void YaraMatch::AddMatch(YaraString^ yaraString)
	{
		this->matchData->Add(yaraString);
	}
}