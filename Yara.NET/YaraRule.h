#pragma once
#include "Stdafx.h"

namespace YaraNET
{
	public ref class YaraRule
	{
	private:
		String^ name;
		List<String^>^ tags;
		String^ namespaceName;
		List<String^>^ stringIdentifiers;
		Dictionary<String^, Object^>^ metas;
	public:

		property String^ Name {
	public:
		String^ get()
		{
			return this->name;
		}
		}

		property List<String^>^ Tags {
	public:
		List<String^>^ get()
		{
			return gcnew List<String^>(this->tags);
		}
		}

		property String^ Namespace {
	public:
		String^ get()
		{
			return this->namespaceName;
		}
		}

		property List<String^>^ StringIdentifiers {
	public:
		List<String^>^ get()
		{
			return gcnew List<String^>(this->stringIdentifiers);
		}
		}

		property Dictionary<String^, Object^>^ Metas {
	public:
		Dictionary<String^, Object^>^ get()
		{
			return gcnew Dictionary<String^, Object^>(this->metas);
		}
		}

		YaraRule(YR_RULE* yaraRulePtr);

		generic <typename T>
			bool ExtractMetaValue(String^ key, T% obj);

			static Dictionary<String^, Object^>^ GetYaraRuleMetaData(YR_RULE* rule);
			static List<String^>^ GetYaraRuleStrings(YR_RULE* rule);
			static List<String^>^ GetYaraRuleTags(YR_RULE* rule);
	};
}
