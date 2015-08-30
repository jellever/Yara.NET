#pragma once

namespace YaraNET
{
	ref class YaraCompilationException : Exception
	{
	private:
		List<YaraCompilationError^>^ yaraCompileErrors;

	public:
		YaraCompilationException(List<YaraCompilationError^>^ compileErrors, String^ message);

		property List<YaraCompilationError^>^ YaraCompileErrors {
	public:
		List<YaraCompilationError^>^ get()
		{
			return gcnew List<YaraCompilationError^>(this->yaraCompileErrors);
		}
		}
	};
}