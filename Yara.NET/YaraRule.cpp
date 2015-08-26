#include "stdafx.h"



YaraRule::YaraRule(YR_RULE* yaraRulePtr)
{
	this->metas = gcnew Dictionary<String^, Object^>();
	this->strings = gcnew List<String^>();
	this->tags = gcnew List<String^>();
	this->name = gcnew String(yaraRulePtr->identifier);
	this->namespaceName = gcnew String(yaraRulePtr->ns->name);
}
