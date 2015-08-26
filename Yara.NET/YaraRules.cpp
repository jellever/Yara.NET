#include "stdafx.h"



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

void YaraRules::ParseNativeYaraRules()
{
	YR_RULE* yaraRulePtr = NULL;
	yr_rules_foreach(this->nativeYaraRulesPtr, yaraRulePtr)
	{
		YaraRule^ yaraRule = gcnew YaraRule(yaraRulePtr);
		this->yaraRules->Add(yaraRule);
	}
}