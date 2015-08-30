#include "stdafx.h"


namespace YaraNET
{
	YaraRule::YaraRule(YR_RULE* yaraRulePtr)
	{
		this->metas = GetYaraRuleMetaData(yaraRulePtr);
		this->stringIdentifiers = GetYaraRuleStrings(yaraRulePtr);
		this->tags = GetYaraRuleTags(yaraRulePtr);
		this->name = gcnew String(yaraRulePtr->identifier);
		this->namespaceName = gcnew String(yaraRulePtr->ns->name);
	}

	List<String^>^ YaraRule::GetYaraRuleTags(YR_RULE* rule)
	{
		List<String^>^ result = gcnew List<String^>();
		const char* tag;
		yr_rule_tags_foreach(rule, tag)
		{
			result->Add(gcnew String(tag));
		}
		return result;
	}

	List<String^>^ YaraRule::GetYaraRuleStrings(YR_RULE* rule)
	{
		List<String^>^ result = gcnew List<String^>();
		YR_STRING* yaraString;
		yr_rule_strings_foreach(rule, yaraString)
		{
			result->Add(gcnew String(yaraString->identifier));
		}
		return result;
	}

	Dictionary<String^, Object^>^ YaraRule::GetYaraRuleMetaData(YR_RULE* rule)
	{
		Dictionary<String^, Object^>^ result = gcnew Dictionary<String^, Object^>();
		YR_META* yaraMeta;
		yr_rule_metas_foreach(rule, yaraMeta)
		{
			if (yaraMeta->identifier == NULL)
				throw gcnew NullReferenceException("Yara Meta identifier may not be empty.");

			String^ metaId = gcnew String(yaraMeta->identifier);

			Object^ value = nullptr;
			switch (yaraMeta->type)
			{
			case META_TYPE_BOOLEAN:
				value = (bool)(yaraMeta->integer == 1);
				break;
			case META_TYPE_INTEGER:
				value = (Int32)yaraMeta->integer;
				break;
			case META_TYPE_STRING:
				value = gcnew String(yaraMeta->string);
				break;
			}
			result->Add(metaId, value);
		}
		return result;
	}

	generic <typename T> bool YaraRule::ExtractMetaValue(String^ key, T% result)
	{
		Object^ value;
		result = T(); //default(T);
		if (this->metas->TryGetValue(key, value))
		{
			Type^ resultType = T::typeid;
			Type^ metaValueType = value->GetType();
			if (metaValueType != resultType)
				throw gcnew ArgumentException(String::Format("Type mismatch between result and meta value type. Result Type: {0}. Meta Value Type: {1}", resultType->Name, metaValueType->Name), "result");
			result = (T)value;
			return true;
		}
		return false;
	}
}