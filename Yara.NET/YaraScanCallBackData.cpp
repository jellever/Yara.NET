#include "stdafx.h"

namespace YaraNET
{
	YaraScanCallBackData::YaraScanCallBackData(IntPtr dataBaseAddress)
	{
		this->dataBaseAddress = dataBaseAddress;
		this->matches = gcnew List<YaraMatch^>();
	}

	void YaraScanCallBackData::AddYaraMatch(YaraMatch^ yaraMatch)
	{
		this->matches->Add(yaraMatch);
	}
}