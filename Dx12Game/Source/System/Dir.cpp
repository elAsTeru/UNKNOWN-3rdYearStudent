#include "Dir.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Helper/StringHelper.h"

// ディレクトリパスの最後 / で終わらない場合 \\ になってしまうので対応させておくと安全
bool GetPaths(const std::string& _DirPath, std::vector<std::string>& _FileNames)
{
	std::filesystem::directory_iterator itr(_DirPath), end;
	std::error_code err;

	for (; itr != end && !err; itr.increment(err)) {
		const std::filesystem::directory_entry entry = *itr;

		_FileNames.push_back(entry.path().string());
		printf("%s\n", _FileNames.back().c_str());
	}

	/* エラー処理 */
	if (err) {
		std::cout << err.value() << std::endl;
		std::cout << err.message() << std::endl;
		return false;
	}
	return true;
}

bool CheckId(const std::string& _FileName, const std::string& _Id)
{
	bool isId = false;	// 識別子か?
	std::string id;		// 識別子

	for (auto itr = _FileName.begin();itr != _FileName.end(); ++itr)
	{
		// 識別子なら記録する
		if (isId)
		{
			id += *itr;
		}
		// '.'が来たら識別子フラグをtrueにする
		else if (*itr == '.')
		{
			isId = true;
		}
	}

	if (id == _Id)
	{
		return true;
	}

	return false;
}

bool CreateEnumClassHeader(const std::string& _FileName, const std::string& _OutDirPath, const std::vector<std::string>& _FileNames)
{
	// ディレクトリ内のファイルが空なら
	if (_FileNames.size() == 0)
	{
		return false;
	}

	std::ofstream outputfile(_OutDirPath + _FileName + ".h");

	// 書き出す文字を格納する
	std::string outData{};

	outData += "#pragma once\n\n";	// #pragma once
	outData += "namespace Res\n";	// namespace Res
	outData += "{\n";				// begin namespace Res

	outData += "\tenum class " + _FileName + ":unsigned char\n";	// enum class name
	outData += "\t{\n";												// begin enum class

	// 列挙開始
	outData += "\t\tBegin = 0,\n\n";

	for (auto filePath : _FileNames)
	{
		std::filesystem::path path = filePath;				// ファイル名を取得するためにファイルシステムでパスを記録
		std::string tempFileName = path.filename().string();	// ファイル名を取得
		std::string fileName{};

		// 識別子を取り除く、'.'が来たらループを終了
		for (auto chara : tempFileName)
		{
			if (chara == '.')
			{
				break;
			}
			fileName += chara;
		}

		// 先頭の文字が小文字なら大文字に変換
		System::Helper::ToUpper(fileName[0]);
		
		outData += "\t\t" + fileName;
		// 先頭要素の場合
		if (filePath == _FileNames[0])
		{
			outData += "=Begin";
		}
		outData += ",\n";
	}

	outData += "\n\t\tEnd\n";
	// 列挙終了


	outData += "\t}; \n";		// end enum class
	outData += "} \n";			// end namespace Res

	// ヘッダファイル書き出し
	outputfile << outData;
	outputfile.close();

	return true;
}