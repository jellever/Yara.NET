using System;
using System.Collections.Generic;
using System.Runtime.Remoting.Channels;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using YaraNET;

namespace Test.Yara.NET
{
    [TestClass]
    public class YaraRuleCompileTest
    {
        [TestMethod]
        public void TestMethod1()
        {
            string ruleSrc = Properties.Resources.TestRule1;
            List<YaraCompilationError> errors;
            using (YaraRules yrRules = YaraNET.Yara.Instance.CompileFromSource(ruleSrc, null, false, null, out errors))
            {
                Assert.IsTrue(errors.Count == 0);
            }
        }
    }
}
