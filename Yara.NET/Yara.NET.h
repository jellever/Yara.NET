// Yara.NET.h

#pragma once
#include "YaraRules.h"
#include "YaraCompilationError.h"


namespace YaraNET 
{	
	[System::Runtime::InteropServices::UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
	public delegate void YR_COMPILER_CALLBACK_FUNC_DELEGATE(int error_level, const char* file_name, int line_number, const char* message, void* user_data);

	public ref class Yara
	{
	private:
		static Yara instance;
		List<YaraCompilationError^>^ yaraCompileErrors;
		Yara();
		Yara(const Yara%) { throw gcnew System::InvalidOperationException("Singleton cannot be copy constructed!"); }		
		YR_COMPILER* CreateYaraCompiler(bool allowIncludes);
		void SetYaraCompilerExternals(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars);
		YaraRules^ CompileYaraRules(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars);
	public:		
		static property Yara^ Instance { Yara^ get() { return %instance; } }		
		void YaraErrorCallback(int error_level, const char* file_name, int line_number, const char* message, void* user_data);
		YaraRules^ CompileFromSource(String^ source, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromSources(Dictionary<String^, String^>^ namespaceSourceDict, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromFile(String^ filePath, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromFiles(Dictionary<String^, String^>^ namespaceFilePathDict, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
	};
}
