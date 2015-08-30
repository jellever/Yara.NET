#pragma once

namespace YaraNET
{
	[System::Runtime::InteropServices::UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	public delegate int YaraScanCallbackDelegate(int message, void* messageDataPtr, void* userDataPtr);

	public ref class YaraRules
	{
	private:
		bool disposed;
		YR_RULES* nativeYaraRulesPtr;
		List<YaraRule^>^ yaraRules;
		Dictionary<String^, Object^>^ externalVars;	

		void ParseNativeYaraRules();
		void SetYaraRulesExternals(Dictionary<String^, Object^>^ externalVars);
		int YaraScanCallback(int message, void* messageDataPtr, void* userDataPtr);
	public:

		property Dictionary<String^, Object^>^ ExternalVariables {
	public:
		Dictionary<String^, Object^>^ get() {
			return gcnew Dictionary<String^, Object^>(this->externalVars);
		}
		}

		property List<YaraRule^>^ Rules {
	public:
		List<YaraRule^>^ get() {
			return gcnew List<YaraRule^>(this->yaraRules);
		}
		}

		YaraRules(YR_RULES* yaraRulesPtr, Dictionary<String^, Object^>^ externalVars);
		~YaraRules();
		!YaraRules();

		List<YaraMatch^>^ MatchData(array<Byte>^ data, IntPtr dataBaseAddress, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout);
		List<YaraMatch^>^ MatchData(IntPtr dataPtr, IntPtr dataSize, IntPtr dataBaseAddress, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout);

		List<YaraMatch^>^ MatchFile(String^ filePath, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout);

		List<YaraMatch^>^ MatchProcessMemory(int pid, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout);
	};
}