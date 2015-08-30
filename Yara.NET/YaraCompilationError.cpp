#include "stdafx.h"

namespace YaraNET
{
	YaraCompilationError::YaraCompilationError(YaraErrorLevel errorLevel, String^ fileName, int lineNr, String^ msg)
	{
		this->errorLevel = errorLevel;
		this->fileName = fileName;
		this->lineNumber = lineNr;
		this->message = msg;
	}
}