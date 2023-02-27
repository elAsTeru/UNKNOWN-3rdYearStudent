#pragma once
// -------------------
// ファイル読み込み用
// -------------------
#include <Rapidjson/document.h>

namespace System::Tool
{
	rapidjson::Document LoadJson(const char* _Path);
}
