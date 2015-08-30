# Yara.NET
Yara.NET is a C++/CLI .NET wrapper around the popular [Yara](http://plusvic.github.io/yara/) project. It currently is compiled against Yara lib 3.4.0. The Yara.NET API was inspired/based on the Yara Python API and thus has a somewhat similar API.

Example:
```C#
string myYaraRule = "...my rules here...";
string namespaceName = null; //optional
List<YaraCompilationError> errors; //contains yara compile warnings. If there are any errors a YaraException is thrown from CompileFromSource
YaraRules yrRules = YaraNET.Yara.Instance.CompileFromSource(myYaraRule, namespaceName, false, null, out errors);
List<YaraMatch^>^ matches = yrRules.MatchFile("C:\mypathtoscan.ext", null, false, 0);
```

Additionaly, MatchProcessMemory and MatchData are available to do Yara matching. These functions use respectively yr_rules_scan_mem, yr_rules_scan_file and yr_rules_scan_proc underwater. 
