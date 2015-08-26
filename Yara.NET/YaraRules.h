#pragma once

#include "YaraRule.h"

public ref class YaraRules
{
private:
	bool disposed;
	YR_RULES* nativeYaraRulesPtr;
	List<YaraRule^>^ yaraRules;
	Dictionary<String^, Object^>^ externalVars;	
	void ParseNativeYaraRules();

public:

	property Dictionary<String^, Object^>^ ExternalVariables{
public:
	Dictionary<String^, Object^>^ get(){
		return gcnew Dictionary<String^, Object^>(this->externalVars);
	}
	}

	YaraRules(YR_RULES* yaraRulesPtr, Dictionary<String^, Object^>^ externalVars);
	~YaraRules();
	!YaraRules();
};

