using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Channels;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using YaraNET;

namespace Test.Yara.NET
{
    [TestClass]
    public class YaraTests
    {
        private YaraRules LoadYaraRulesFromSource(string ruleSrc, string namespaceName)
        {
            List<YaraCompilationError> errors;
            YaraRules yrRules = YaraNET.Yara.Instance.CompileFromSource(ruleSrc, namespaceName, false, null, out errors);
            Assert.IsNotNull(yrRules);
            Assert.IsNotNull(errors);
            Assert.IsTrue(errors.Count == 0);
            return yrRules;
        }

        private void TestExtractMeta<T>(YaraRule rule, string metaIdentifier, T expectedValue)
        {
            T metaValue = default(T);
            bool metaExtractSuccess = rule.ExtractMetaValue(metaIdentifier, ref metaValue);
            Assert.IsTrue(metaExtractSuccess);
            Assert.AreEqual(expectedValue, metaValue);
        }

        private void TestRuleContainsStringIdentifier(YaraRule rule, string stringIdentifier)
        {
            bool containsIdentifier = rule.StringIdentifiers.Contains(stringIdentifier);
            Assert.IsTrue(containsIdentifier);
        }

        [TestMethod]
        public void TestYaraLoadFromSource()
        {
            string yrRuleSource = Properties.Resources.TestRule1;
            using (YaraRules yrRules = LoadYaraRulesFromSource(yrRuleSource, null))
            {
                Assert.AreEqual(yrRules.Rules.Count, 1);

                YaraRule rule = yrRules.Rules[0];
                Assert.AreEqual("silent_banker", rule.Name);
                Assert.AreEqual("default", rule.Namespace);

                Assert.AreEqual(3, rule.Metas.Count);
                TestExtractMeta(rule, "description", "This is just an example");
                TestExtractMeta(rule, "thread_level", 3);
                TestExtractMeta(rule, "in_the_wild", true);

                TestRuleContainsStringIdentifier(rule, "$a");
                TestRuleContainsStringIdentifier(rule, "$b");
                TestRuleContainsStringIdentifier(rule, "$c");

                Assert.AreEqual(yrRules.Rules[0].StringIdentifiers.Count, 3);
            }
        }

        [TestMethod]
        public void TestYaraLoadFaultyRule()
        {
            try
            {
                string yrRuleSource = Properties.Resources.FaultyRule;
                using (YaraRules yrRules = LoadYaraRulesFromSource(yrRuleSource, null))
                {
                }
                Assert.Fail("Exptected a Yara exception!");
            }
            catch (YaraCompilationException ex)
            {
                Assert.AreEqual(1, ex.YaraCompileErrors.Count);
                YaraCompilationError compileErr = ex.YaraCompileErrors[0];
                Assert.AreEqual(YaraErrorLevel.Error, compileErr.ErrorLevel);
                Assert.AreEqual("syntax error, unexpected $end, expecting '}'", compileErr.Message);
            }
        }

        private void TestYaraMatchContainsStringData(YaraMatch yaraMatch, string stringIdentifier, int expectedNrOfStringMatches)
        {
            IEnumerable<YaraString> strings = yaraMatch.MatchData.Where(x => x.IdentifierName == stringIdentifier);
            Assert.AreEqual(expectedNrOfStringMatches, strings.Count());
        }

        [TestMethod]
        public void TestYaraBasicMatch()
        {
            string yrRuleSource = Properties.Resources.TestRule1;
            using (YaraRules yrRules = LoadYaraRulesFromSource(yrRuleSource, null))
            {
                using (MemoryStream memStream = new MemoryStream())
                using (StreamWriter streamWriter = new StreamWriter(memStream))
                {
                    string testRule1Data = Properties.Resources.TestRule1Data;
                    streamWriter.Write(testRule1Data);
                    streamWriter.Flush();
                    byte[] memStreamData = memStream.ToArray();
                    List<YaraMatch> matches =  yrRules.MatchData(memStreamData, IntPtr.Zero, null, false, 0);
                    Assert.AreEqual(1, matches.Count);

                    YaraMatch match = matches[0];
                    Assert.IsNotNull(match.Rule);
                    Assert.AreEqual("silent_banker", match.Rule.Name);
                    Assert.IsTrue(match.ContainsMatches);
                    TestYaraMatchContainsStringData(match, "$a", 1);
                    TestYaraMatchContainsStringData(match, "$b", 1);
                    TestYaraMatchContainsStringData(match, "$c", 3);
                }
            }
        }

       
    }
}
