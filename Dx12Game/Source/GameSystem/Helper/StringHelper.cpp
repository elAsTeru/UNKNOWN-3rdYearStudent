#include "StringHelper.h"
#include <assert.h>
#include <d3d12.h>
#include <algorithm>

namespace Sys::Helper
{
	std::wstring ToWstr(const std::string& _Str)
	{
		// 呼び出し１回目(文字列数を得る)
		auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _Str.c_str(), -1, nullptr, 0);

		std::wstring wstr;
		wstr.resize(num1);		// 得られた文字列数でリサイズ

		// 呼び出し２回目(確保済みのwstrに変換文字列をコピー)
		auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, _Str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);	// 一応チェック
		return wstr;
	}

	std::string ToStr(const std::wstring& _Wstr)
	{
		// 呼び出し1回目(文字列数を得る)
		auto num1 = WideCharToMultiByte(CP_UTF8, 0U, _Wstr.data(), -1, nullptr, 0, nullptr, nullptr);

		std::string str;
		str.resize(num1);

		auto num2 = WideCharToMultiByte(CP_UTF8, 0U, _Wstr.data(), -1, &str[0], num1, nullptr, nullptr);

		assert(num1 == num2);	// 一応チェック
		return str;
	}

	void ToUpper(char& _Chara)
	{
		// 小文字なら
		if ('a' <= _Chara && _Chara <= 'z')
		{
			_Chara -= 'a' - 'A';
		}
	}

	void ToLower(char& _Chara)
	{
		// 大文字なら
		if ('A' <= _Chara && _Chara <= 'Z')
		{
			_Chara += 'a' - 'A';
		}
	}

	void FillZeroLeft(std::wstring& _Str, const int& _Digits)
	{
		// 実行回数
		const int8_t runNum = _Digits - _Str.length();

		// 指定した桁数に満たない分'0'を挿入
		for (uint8_t i = 0; i < runNum; ++i)
		{
			_Str.insert(_Str.begin(), '0');
		}
	}

	void InsertPartition(std::wstring& _Str, const std::wstring& _InsertStr, const int& _Space)
	{
		uint8_t length = _Str.length();								// 始めの文字列長を記録
		std::wstring revInsertStr = _InsertStr;						// 挿入する文字列を反転した文字列を格納する用
		std::reverse(revInsertStr.begin(), revInsertStr.end());

		for (uint8_t i = 1; i <= length; ++i)
		{
			if (i % _Space == 0)
			{
				for (auto chara : revInsertStr)
				{
					_Str.insert(_Str.begin() + (length - i), chara);
				}
			}
		}
	}
}