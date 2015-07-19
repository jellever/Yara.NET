#include "stdafx.h"
#include "YaraRules.h"
#include "yara.h"

using namespace System;
using namespace System::Collections::Generic;

YaraRules::YaraRules(YR_RULES* yaraRulesPtr, Dictionary<String^, Object^>^ externalVars)
{
	this->yaraRules = gcnew List<YaraRule^>();
	this->externalVars = externalVars;
	this->nativeYaraRulesPtr = yaraRulesPtr;
	this->ParseNativeYaraRules();
}

YaraRules::~YaraRules()
{
	if (this->disposed)
		return;
	this->disposed = true;
	this->!YaraRules();	
}

YaraRules::!YaraRules()
{
	if (this->nativeYaraRulesPtr != NULL)
	{
		yr_rules_destroy(this->nativeYaraRulesPtr);
		this->nativeYaraRulesPtr = NULL;
	}
}