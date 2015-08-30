#include "stdafx.h"


namespace YaraNET
{
	YaraRules::YaraRules(YR_RULES* yaraRulesPtr, Dictionary<String^, Object^>^ externalVars)
	{
		this->yaraRules = gcnew List<YaraRule^>();
		if (externalVars != nullptr)
		{
			this->externalVars = externalVars;
		}
		else
		{
			this->externalVars = gcnew Dictionary<String^, Object^>();
		}
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

	void YaraRules::SaveCompiledRules(String^ filePath)
	{
		marshal_context marshalCtx;
		const char* filePathStr = marshalCtx.marshal_as<const char*>(filePath);
		int yrErr = yr_rules_save(this->nativeYaraRulesPtr, filePathStr);
		if (yrErr != ERROR_SUCCESS)
			throw gcnew YaraException(yrErr, "Failed to save compiled Yara ruled!");
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

	void YaraRules::SetYaraRulesExternals(Dictionary<String^, Object^>^ externalVars)
	{
		marshal_context marshalCtx;
		for each (auto nameValuePair in externalVars)
		{
			int error = ERROR_SUCCESS;
			Type^ valueType = nameValuePair.Value->GetType();
			const char* valueNamePtr = marshalCtx.marshal_as<const char*>(nameValuePair.Key);

			if (valueType == Boolean::typeid)
			{
				error = yr_rules_define_boolean_variable(this->nativeYaraRulesPtr, valueNamePtr, (bool)nameValuePair.Value);
			}
			else if (valueType == Int64::typeid || valueType == Int32::typeid)
			{
				error = yr_rules_define_integer_variable(this->nativeYaraRulesPtr, valueNamePtr, (Int64)nameValuePair.Value);
			}
			else if (valueType == Double::typeid)
			{
				error = yr_rules_define_float_variable(this->nativeYaraRulesPtr, valueNamePtr, (double)nameValuePair.Value);
			}
			else if (valueType == String::typeid)
			{
				const char* varValuePtr = marshalCtx.marshal_as<const char*>((String^)nameValuePair.Value);
				error = yr_rules_define_string_variable(this->nativeYaraRulesPtr, valueNamePtr, varValuePtr);
			}
			else
			{
				throw gcnew NotSupportedException(String::Format("External variable type '{0}' is not supported.", valueType->Name));
			}

			if (error != ERROR_SUCCESS)
			{
				throw gcnew YaraException(error, "Failed to add external variable to Yara compiler.");
			}
		}
	}

	int YaraRules::YaraScanCallback(int message, void* messageDataPtr, void* userDataPtr)
	{
		GCHandle callbackDataHandle = GCHandle::FromIntPtr((IntPtr)userDataPtr);
		YaraScanCallBackData^ callbackData = (YaraScanCallBackData^)callbackDataHandle.Target;
		int callbackResult = CALLBACK_CONTINUE;
		if (message == CALLBACK_MSG_RULE_MATCHING)
		{
			YR_RULE* rule = (YR_RULE*)messageDataPtr;
			YaraMatch^ yaraMatch = gcnew YaraMatch(gcnew YaraRule(rule));
			YR_STRING* yaraString;
			yr_rule_strings_foreach(rule, yaraString)
			{
				YR_MATCH* match;
				yr_string_matches_foreach(yaraString, match)
				{
					IntPtr memoryAddress = (IntPtr)(callbackData->DataBaseAddress.ToInt64() + match->offset);
					IntPtr matchSize = (IntPtr)match->length;
					IntPtr matchOffset = (IntPtr)match->offset;
					YaraString^ netYaraString = gcnew YaraString(match->data, matchSize, matchOffset, memoryAddress, gcnew String(yaraString->identifier));
					yaraMatch->AddMatch(netYaraString);
				}
			}
			callbackData->AddYaraMatch(yaraMatch);
		}
		return callbackResult;
	}

	List<YaraMatch^>^ YaraRules::MatchData(IntPtr dataPtr, IntPtr dataSize, IntPtr dataBaseAddress, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout)
	{
		bool restoreExternalVariables = false;
		GCHandle callbackDataHandle;
		GCHandle scanCallbackDelegateHandle;
		int err = ERROR_SUCCESS;
		try
		{
			if (externalVars != nullptr)
			{
				restoreExternalVariables = true;
				this->SetYaraRulesExternals(externalVars);
			}

			int scanFlags = fast ? SCAN_FLAGS_FAST_MODE : 0;

			YaraScanCallBackData^ callbackData = gcnew YaraScanCallBackData(dataBaseAddress);
			callbackDataHandle = GCHandle::Alloc(callbackData);
			void* callbackDataPtr = GCHandle::ToIntPtr(callbackDataHandle).ToPointer();

			YaraScanCallbackDelegate^ scanCallbackDelegate = gcnew YaraScanCallbackDelegate(this, &YaraRules::YaraScanCallback);
			scanCallbackDelegateHandle = GCHandle::Alloc(scanCallbackDelegate);
			YR_CALLBACK_FUNC yrCallbackFunc = (YR_CALLBACK_FUNC)Marshal::GetFunctionPointerForDelegate(scanCallbackDelegate).ToPointer();

			err = yr_rules_scan_mem(this->nativeYaraRulesPtr, (uint8_t*)dataPtr.ToPointer(), (size_t)dataSize.ToInt64(), scanFlags, yrCallbackFunc, callbackDataPtr, timeout);

			if (err == ERROR_SUCCESS)
			{
				return callbackData->Matches;
			}
			else
			{
				throw gcnew YaraException(err, "Yara failed to scan given memory.");
			}
		}
		finally
		{
			if (callbackDataHandle.IsAllocated)
				callbackDataHandle.Free();
			if (scanCallbackDelegateHandle.IsAllocated)
				scanCallbackDelegateHandle.Free();
			if (restoreExternalVariables)
			{
				this->SetYaraRulesExternals(this->externalVars);
			}
		}
	}

	List<YaraMatch^>^ YaraRules::MatchData(array<Byte>^ data, IntPtr dataBaseAddress, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout)
	{
		pin_ptr<Byte> pinnedData = &data[0];
		IntPtr dataPtr = (IntPtr)(void*)pinnedData;
		return this->MatchData(dataPtr, (IntPtr)data->Length, dataBaseAddress, externalVars, fast, timeout);
	}

	List<YaraMatch^>^ YaraRules::MatchFile(String^ filePath, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout)
	{
		bool restoreExternalVariables = false;
		GCHandle callbackDataHandle;
		GCHandle scanCallbackDelegateHandle;
		int err = ERROR_SUCCESS;
		try
		{
			if (externalVars != nullptr)
			{
				restoreExternalVariables = true;
				this->SetYaraRulesExternals(externalVars);
			}

			int scanFlags = fast ? SCAN_FLAGS_FAST_MODE : 0;

			YaraScanCallBackData^ callbackData = gcnew YaraScanCallBackData(IntPtr::Zero);
			callbackDataHandle = GCHandle::Alloc(callbackData);
			void* callbackDataPtr = GCHandle::ToIntPtr(callbackDataHandle).ToPointer();

			YaraScanCallbackDelegate^ scanCallbackDelegate = gcnew YaraScanCallbackDelegate(this, &YaraRules::YaraScanCallback);
			scanCallbackDelegateHandle = GCHandle::Alloc(scanCallbackDelegate);
			YR_CALLBACK_FUNC yrCallbackFunc = (YR_CALLBACK_FUNC)Marshal::GetFunctionPointerForDelegate(scanCallbackDelegate).ToPointer();

			marshal_context context;
			const char* filePathStr = context.marshal_as<const char*>(filePath);

			err = yr_rules_scan_file(this->nativeYaraRulesPtr, filePathStr, scanFlags, yrCallbackFunc, callbackDataPtr, timeout);

			if (err == ERROR_SUCCESS)
			{
				return callbackData->Matches;
			}
			else
			{
				throw gcnew YaraException(err, "Yara failed to scan given memory.");
			}
		}
		finally
		{
			if (callbackDataHandle.IsAllocated)
				callbackDataHandle.Free();
			if (scanCallbackDelegateHandle.IsAllocated)
				scanCallbackDelegateHandle.Free();
			if (restoreExternalVariables)
			{
				this->SetYaraRulesExternals(this->externalVars);
			}
		}
	}


	List<YaraMatch^>^ YaraRules::MatchProcessMemory(int pid, Dictionary<String^, Object^>^ externalVars, bool fast, int timeout)
	{
		bool restoreExternalVariables = false;
		GCHandle callbackDataHandle;
		GCHandle scanCallbackDelegateHandle;
		int err = ERROR_SUCCESS;
		try
		{
			if (externalVars != nullptr)
			{
				restoreExternalVariables = true;
				this->SetYaraRulesExternals(externalVars);
			}

			int scanFlags = fast ? SCAN_FLAGS_FAST_MODE : 0;

			YaraScanCallBackData^ callbackData = gcnew YaraScanCallBackData(IntPtr::Zero);
			callbackDataHandle = GCHandle::Alloc(callbackData);
			void* callbackDataPtr = GCHandle::ToIntPtr(callbackDataHandle).ToPointer();

			YaraScanCallbackDelegate^ scanCallbackDelegate = gcnew YaraScanCallbackDelegate(this, &YaraRules::YaraScanCallback);
			scanCallbackDelegateHandle = GCHandle::Alloc(scanCallbackDelegate);
			YR_CALLBACK_FUNC yrCallbackFunc = (YR_CALLBACK_FUNC)Marshal::GetFunctionPointerForDelegate(scanCallbackDelegate).ToPointer();

			err = yr_rules_scan_proc(this->nativeYaraRulesPtr, pid, scanFlags, yrCallbackFunc, callbackDataPtr, timeout);

			if (err == ERROR_SUCCESS)
			{
				return callbackData->Matches;
			}
			else
			{
				throw gcnew YaraException(err, "Yara failed to scan given memory.");
			}
		}
		finally
		{
			if (callbackDataHandle.IsAllocated)
				callbackDataHandle.Free();
			if (scanCallbackDelegateHandle.IsAllocated)
				scanCallbackDelegateHandle.Free();
			if (restoreExternalVariables)
			{
				this->SetYaraRulesExternals(this->externalVars);
			}
		}
	}
}