#pragma once
#include <string>

namespace Sys::Helper
{
	/// <summary> string(マルチバイト文字列)からwstring(ワイド文字列)を返却 </summary>
	std::wstring ToWstr(const std::string& _Str);

	/// <summary> wstring(ワイド文字列)からstring(マルチバイト文字列)を返却 </summary>
	std::string ToStr(const std::wstring& _Wstr);

	/// <summary> 小文字を大文字に変換 </summary>
	void ToUpper(char& _Chara);
	/// <summary> 大文字を小文字に変換 </summary>
	void ToLower(char& _Chara);

	/// <summary> 指定した桁数に満たない分、左側を0で埋めて返却。マルチバイト文字列用は必要になったら作成する。 </summary>
	/// <param name="_ValStr">文字列</param>
	/// <param name="_Digits">桁数</param>
	void FillZeroLeft(std::wstring& _Str, const int& _Digits);

	/// <summary> 指定した間隔で指定した文字列を挿入し返却。マルチバイト文字列用は必要になったら作成する。</summary>
	/// <param name="_InsertStr">文字</param>
	/// <param name="_Space">間隔</param>
	void InsertPartition(std::wstring& _Str, const std::wstring& _InsertStr, const int& _Space);
}