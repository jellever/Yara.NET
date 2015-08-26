#pragma once
#include "yara.h"


public ref class YaraRule
{
private:
	String^ name;
	List<String^>^ tags;
	String^ namespaceName;
	List<String^>^ strings;
	Dictionary<String^, Object^>^ metas;
public:

	property String^ Name{
public:
	String^ get()
	{
		return this->name;
	}
	}

	property List<String^>^ Tags{
public:
	List<String^>^ get()
	{
		return gcnew List<String^>(this->tags);
	}
	}

	property String^ Namespace{
public:
	String^ get()
	{
		return this->namespaceName;
	}
	}

	property List<String^>^ Strings{
public:
	List<String^>^ get()
	{
		return gcnew List<String^>(this->strings);
	}
	}

	property Dictionary<String^, Object^>^ Metas{
public:
	Dictionary<String^, Object^>^ get()
	{
		return gcnew Dictionary<String^, Object^>(this->metas);
	}
	}

	YaraRule(YR_RULE* yaraRulePtr);
};

