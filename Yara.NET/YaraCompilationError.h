#pragma once

namespace YaraNET
{
	public enum class YaraErrorLevel
	{
		Error = 0,
		Warning = 1
	};

	public ref class YaraCompilationError
	{
	private:
		YaraErrorLevel errorLevel;
		String^ fileName;
		int lineNumber;
		String^ message;
		IntPtr userData;

	public:

		property YaraErrorLevel ErrorLevel
		{
		public:
			YaraErrorLevel get() {
				return errorLevel;
			}
		}

		property String^ FileName
		{
		public:
			String^ get() {
				return fileName;
			}
		}

		property int LineNumber
		{
		public:
			int get() {
				return lineNumber;
			}
		}

		property String^ Message
		{
		public:
			String^ get() {
				return message;
			}
		}

		YaraCompilationError(YaraErrorLevel errorLevel, String^ fileName, int LineNr, String^ msg);
	};

}