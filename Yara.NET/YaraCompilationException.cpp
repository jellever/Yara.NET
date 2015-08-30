#include "stdafx.h"
#include "YaraCompilationException.h"


namespace YaraNET
{
	YaraCompilationException::YaraCompilationException(List<YaraCompilationError^>^ compileErrors, String^ message)
		:Exception(message)
	{
		this->yaraCompileErrors = gcnew List<YaraCompilationError^>(compileErrors);
	}
}