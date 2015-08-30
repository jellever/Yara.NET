// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include <msclr\marshal.h>
#include <msclr\lock.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

#include "yara.h"
#include "YaraCompilationError.h"
#include "YaraCompilationException.h"
#include "YaraException.h"
#include "YaraRule.h"
#include "YaraString.h"
#include "YaraMatch.h"
#include "YaraScanCallBackData.h"
#include "YaraRules.h"
#include "Yara.NET.h"


