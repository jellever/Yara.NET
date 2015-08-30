// This is the main DLL file.

#include "stdafx.h"

namespace YaraNET
{
	Yara::Yara()
	{
		yaraCompileErrors = gcnew List<YaraCompilationError^>();
		int err = yr_initialize();
		if (err != ERROR_SUCCESS)
		{
			throw gcnew YaraException(err, "Failed to initialize Yara library!");
		}
	}

	Yara::!Yara()
	{
		yr_finalize();
	}

	YaraRules^ Yara::LoadCompiledRules(String^ filePath)
	{
		Dictionary<String^, Object^>^ externalVariables = gcnew Dictionary<String^, Object^>();
		marshal_context marshalCtx;
		const char* filePathStr = marshalCtx.marshal_as<const char*>(filePath);

		YR_RULES* yaraRulesPtr = NULL;
		YR_EXTERNAL_VARIABLE* yaraExternalVarPtr = NULL;
		int yrErr = yr_rules_load(filePathStr, &yaraRulesPtr);
		if (yrErr != ERROR_SUCCESS)
			throw gcnew YaraException(yrErr, String::Format("Failed to load compiled Yara rules from file '{0}'.", filePath));

		yaraExternalVarPtr = yaraRulesPtr->externals_list_head;
		while (!EXTERNAL_VARIABLE_IS_NULL(yaraExternalVarPtr))
		{
			char* externalVarIdentifier = yaraExternalVarPtr->identifier;
			String^ externalVarIdentifierString = gcnew String(externalVarIdentifier);
			Object^ externalVarValue = nullptr;

			switch (yaraExternalVarPtr->type)
			{
			case EXTERNAL_VARIABLE_TYPE_STRING:
				externalVarValue = gcnew String(yaraExternalVarPtr->value.s);
				break;
			case EXTERNAL_VARIABLE_TYPE_BOOLEAN:
				externalVarValue = (bool)(yaraExternalVarPtr->value.i == 1);
				break;
			case EXTERNAL_VARIABLE_TYPE_INTEGER:
				externalVarValue = (Int64)yaraExternalVarPtr->value.i;
				break;
			case EXTERNAL_VARIABLE_TYPE_FLOAT:
				externalVarValue = (double)yaraExternalVarPtr->value.f;
				break;
			}
			externalVariables->Add(externalVarIdentifierString, externalVarValue);
			yaraExternalVarPtr++;
		}
		YaraRules^ rules = gcnew YaraRules(yaraRulesPtr, externalVariables);
		return rules;
	}

	YR_COMPILER* Yara::CreateYaraCompiler(bool allowIncludes)
	{
		YR_COMPILER* result;
		int err = yr_compiler_create(&result);
		if (err == ERROR_SUCCESS)
		{
			result->allow_includes = allowIncludes;
			return result;
		}
		else
		{
			throw gcnew YaraException(err, "Failed to create YR_COMPILER!");
		}
	}

	void Yara::SetYaraCompilerExternals(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars)
	{
		marshal_context marshalCtx;
		for each (auto nameVarValueKeyPair in externalVars)
		{
			int err = ERROR_SUCCESS;
			const char* varNamePtr = marshalCtx.marshal_as<const char*>(nameVarValueKeyPair.Key);
			Type^ varType = nameVarValueKeyPair.Value->GetType();
			if (varType == Boolean::typeid)
			{
				err = yr_compiler_define_boolean_variable(compiler, varNamePtr, (bool)nameVarValueKeyPair.Value);
			}
			else if (varType == Int64::typeid || varType == Int32::typeid)
			{
				err = yr_compiler_define_integer_variable(compiler, varNamePtr, (Int64)nameVarValueKeyPair.Value);
			}
			else if (varType == Double::typeid)
			{
				err = yr_compiler_define_float_variable(compiler, varNamePtr, (double)nameVarValueKeyPair.Value);
			}
			else if (varType == String::typeid)
			{
				const char* varValuePtr = marshalCtx.marshal_as<const char*>((String^)nameVarValueKeyPair.Value);
				err = yr_compiler_define_string_variable(compiler, varNamePtr, varValuePtr);
			}
			else
			{
				throw gcnew NotSupportedException(String::Format("External variable type '{0}' is not supported.", varType->Name));
			}

			if (err != ERROR_SUCCESS)
			{
				throw gcnew YaraException(err, "Failed to add external variable to Yara compiler.");
			}
		}
	}

	void Yara::YaraErrorCallback(int error_level, const char* file_name, int line_number, const char* message, void* user_data)
	{
		String^ filename = gcnew String(file_name);
		String^ msg = gcnew String(message);
		YaraCompilationError^ yaraError = gcnew YaraCompilationError((YaraErrorLevel)error_level, filename, line_number, msg);
		this->yaraCompileErrors->Add(yaraError);
	}

	YaraRules^ Yara::CompileYaraRules(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars)
	{
		YR_RULES* yaraRulesPtr = NULL;
		int err = yr_compiler_get_rules(compiler, &yaraRulesPtr);
		if (err != ERROR_SUCCESS)
			throw gcnew YaraException(err, "Failed to compile Yara rules!");
		return gcnew YaraRules(yaraRulesPtr, externalVars);
	}

	YaraRules^ Yara::CompileFromSource(String^ source, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings)
	{
		List<KeyValuePair<String^, String^>>^ namespaceSourceList = gcnew List<KeyValuePair<String^, String^>>();
		namespaceSourceList->Add(KeyValuePair<String^, String^>(namespace_, source));
		return CompileFromSources(namespaceSourceList, allowIncludes, externalVars, compileWarnings);
	}

	YaraRules^ Yara::CompileFromSources(List<KeyValuePair<String^, String^>>^ namespaceSourceList, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings)
	{
		compileWarnings = gcnew List<YaraCompilationError^>();
		marshal_context marshalCtx;
		GCHandle delHandle;
		YR_COMPILER* yaraCompiler = CreateYaraCompiler(allowIncludes);

		try
		{
			if (externalVars != nullptr)
				SetYaraCompilerExternals(yaraCompiler, externalVars);

			YR_COMPILER_CALLBACK_FUNC_DELEGATE^ callbackDelegate = gcnew YR_COMPILER_CALLBACK_FUNC_DELEGATE(this, &Yara::YaraErrorCallback);
			delHandle = GCHandle::Alloc(callbackDelegate);
			IntPtr callbackPtr = Marshal::GetFunctionPointerForDelegate(callbackDelegate);
			yr_compiler_set_callback(yaraCompiler, (YR_COMPILER_CALLBACK_FUNC)callbackPtr.ToPointer(), NULL);

			for each (auto namespaceSourcePair in namespaceSourceList)
			{
				const char* namespacePtr = marshalCtx.marshal_as<const char*>(namespaceSourcePair.Key);
				const char* sourcePtr = marshalCtx.marshal_as<const char*>(namespaceSourcePair.Value);
				int errCount = yr_compiler_add_string(yaraCompiler, sourcePtr, namespacePtr);
				if (errCount > 0)
					throw gcnew YaraCompilationException(this->yaraCompileErrors, "Failed to add source to Yara compiler");
				compileWarnings->AddRange(this->yaraCompileErrors);
			}

			return CompileYaraRules(yaraCompiler, externalVars);
		}
		finally{
			this->yaraCompileErrors->Clear();
			if (delHandle.IsAllocated)
				delHandle.Free();
			if (yaraCompiler != NULL)
			{
				yr_compiler_destroy(yaraCompiler);
				yaraCompiler = NULL;
			}
		}
	}

	YaraRules^ Yara::CompileFromFile(String^ filePath, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, List<YaraCompilationError^>^% compileWarnings)
	{
		List<KeyValuePair<String^, String^>>^ namespaceFilePathList = gcnew List<KeyValuePair<String^, String^>>();
		namespaceFilePathList->Add(KeyValuePair<String^, String^>(namespace_, filePath));
		return CompileFromFiles(namespaceFilePathList, allowIncludes, externalVars, compileWarnings);
	}

	YaraRules^ Yara::CompileFromFiles(List<KeyValuePair<String^, String^>>^ namespaceFilePathList, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, List<YaraCompilationError^>^% compileWarnings)
	{
		FILE* yaraSourceFile = NULL;
		compileWarnings = gcnew List<YaraCompilationError^>();
		marshal_context marshalCtx;
		GCHandle delHandle;
		YR_COMPILER* yaraCompiler = CreateYaraCompiler(allowIncludes);

		try
		{
			if (externalVars != nullptr)
				SetYaraCompilerExternals(yaraCompiler, externalVars);

			YR_COMPILER_CALLBACK_FUNC_DELEGATE^ callbackDelegate = gcnew YR_COMPILER_CALLBACK_FUNC_DELEGATE(this, &Yara::YaraErrorCallback);
			delHandle = GCHandle::Alloc(callbackDelegate);
			IntPtr callbackPtr = Marshal::GetFunctionPointerForDelegate(callbackDelegate);
			yr_compiler_set_callback(yaraCompiler, (YR_COMPILER_CALLBACK_FUNC)callbackPtr.ToPointer(), NULL);

			for each (auto namespaceFilePair in namespaceFilePathList)
			{
				const char* namespacePtr = marshalCtx.marshal_as<const char*>(namespaceFilePair.Key);
				const char* filePathStr = marshalCtx.marshal_as<const char*>(namespaceFilePair.Value);

				yaraSourceFile = fopen(filePathStr, "r");
				if (yaraSourceFile != NULL)
				{
					int errCount = yr_compiler_add_file(yaraCompiler, yaraSourceFile, namespacePtr, filePathStr);
					fclose(yaraSourceFile);
					yaraSourceFile = NULL;
					if (errCount > 0)
						throw gcnew YaraCompilationException(this->yaraCompileErrors, "Failed to add source to Yara compiler");
					compileWarnings->AddRange(this->yaraCompileErrors);
				}
				else
				{
					throw gcnew System::IO::IOException(String::Format("Failed to open Yara Source file '{0}'", namespaceFilePair.Key));
				}
			}

			return CompileYaraRules(yaraCompiler, externalVars);
		}
		finally{
			this->yaraCompileErrors->Clear();
			if (yaraSourceFile != NULL)
			{
				fclose(yaraSourceFile);
				yaraSourceFile = NULL;
			}
			if (delHandle.IsAllocated)
				delHandle.Free();
			if (yaraCompiler != NULL)
			{
				yr_compiler_destroy(yaraCompiler);
				yaraCompiler = NULL;
			}
		}
	}


}