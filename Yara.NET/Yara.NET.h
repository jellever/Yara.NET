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
		!Yara();
		YR_COMPILER* CreateYaraCompiler(bool allowIncludes);
		void YaraErrorCallback(int error_level, const char* file_name, int line_number, const char* message, void* user_data);
		void SetYaraCompilerExternals(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars);
		YaraRules^ CompileYaraRules(YR_COMPILER* compiler, Dictionary<String^, Object^>^ externalVars);
	public:
		/// <summary>
		/// Yara singleton instance
		/// </summary>
		static property Yara^ Instance { Yara^ get() { return %instance; } }

		/// <summary>
		/// Load compiled Yara rules from file.
		/// </summary>
		/// <param name="filePath">The file to load the compiled rules from.</param>
		/// <returns>YaraRules instance</returns>
		YaraRules^ LoadCompiledRules(String^ filePath);

		/// <summary>
		/// Compile Yara rules from source
		/// </summary>
		/// <param name="source">The Yara rule source</param>
		/// <param name="namespace_">The namespace to put the Yara rules in</param>
		/// <param name="allowIncludes"></param>
		/// <param name="externalVars"></param>
		/// <param name="compileWarnings"></param>
		/// <returns></returns>
		YaraRules^ CompileFromSource(String^ source, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromSources(List<KeyValuePair<String^, String^>>^ namespaceSourceList, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromFile(String^ filePath, String^ namespace_, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
		YaraRules^ CompileFromFiles(List<KeyValuePair<String^, String^>>^ namespaceFilePathList, bool allowIncludes, Dictionary<String^, Object^>^ externalVars, [Out] List<YaraCompilationError^>^% compileWarnings);
	};
}
