#pragma once
// -------------------
// ファイル読み込み用
// -------------------
#include <Rapidjson/document.h>

namespace Sys::Tool
{
	rapidjson::Document LoadJson(const char* _Path);
}
