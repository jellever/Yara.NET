#include "stdafx.h"


namespace YaraNET
{
	YaraException::YaraException(int err, String^ message)
		: Exception(String::Format("{0}\nYara Error: {1}", message, err))
	{
		this->yaraError = err;
	}
}