#pragma once
#include <string>

namespace System::Helper
{
	/// <summary> string(マルチバイト文字列)からwstring(ワイド文字列)を返却 </summary>
	std::wstring ToWstr(const std::string& _Str);

	/// <summary> wstring(ワイド文字列)からstring(マルチバイト文字列)を返却 </summary>
	std::string ToStr(const std::wstring& _Wstr);

	/// <summary> 小文字を大文字に変換 </summary>
	void ToUpper(char& _Chara);
	/// <summary> 大文字を小文字に変換 </summary>
	void ToLower(char& _Chara);
}