#pragma once

#include <string>
#include <Shlwapi.h>

#pragma comment( lib, "shlwapi.lib ")

/// <summary>
/// ファイルパス検索
/// </summary>
/// <param name="filename">検索するファイルパス</param>
/// <param name="result">検索結果の格納先</param>
/// <returns>true:ファイル発見 / false:ファイルが見つからなかった</returns>
/// <memo>
/// 検索ルールは以下の通り
/// .\
///..\
///..\..\
///.\res\
///%EXE_DIR%\
///%EXE_DIR%\..\
///%EXE_DIR%\..\..\
///%EXE_DIR%\res\
/// </memo>
 bool  SearchFilePathA(const char* filename, std::string& result);

/// <summary>
/// ファイルパス検索
/// </summary>
/// <param name="filename">検索するファイルパス</param>
/// <param name="result">検索結果の格納先</param>
/// <returns>true:ファイル発見 / false:ファイルが見つからなかった</returns>
/// <memo>
/// 検索ルールは以下の通り
///.\
///..\
///..\..\
///.\res\
///%EXE_DIR%\
///%EXE_DIR%\..\
///%EXE_DIR%\..\..\
///%EXE_DIR%\res\
/// </memo>
 bool  SearchFilePathW(const wchar_t* filename, std::wstring& result);

/// <summary>
/// ディレクトリパスを削除し、ファイルパスを返却
/// </summary>
/// <param name="path">ディレクトリパスを取り除くファイルパス</param>
/// <returns>ファイル名を返却</returns>
 std::string  RemoveDirectoryPathA(const std::string& path);

/// <summary>
/// ディレクトリパスを削除し、ファイルパスを返却
/// </summary>
/// <param name="path">ディレクトリパスを取り除くファイルパス</param>
/// <returns>ファイル名を返却</returns>
 std::wstring  RemoveDirectoryPathW(const std::wstring& path);

/// <summary>
/// ディレクトリ名を取得
/// </summary>
/// <param name="path">ファイルパス</param>
/// <returns>指定されたファイルパスからディレクトリ名を抜き出す</returns>
 std::string  GetDirectoryPathA(const char* path);

/// <summary>
/// ディレクトリ名を取得
/// </summary>
/// <param name="path">ファイルパス</param>
/// <returns>指定されたファイルパスからディレクトリ名を抜き出す</returns>
 std::wstring  GetDirectoryPathW(const wchar_t* path);


#if defined(UNICODE) || defined(_UNICODE)
 inline bool  SearchFilePath(const char* filename, std::string& result)
 {
     return SearchFilePathA(filename, result);
 }

 inline std::string  RemoveDirectoryPath(const std::string& path)
 {
     return RemoveDirectoryPathA(path);
 }

 inline std::string  GetDirectoryPath(const char* path)
 {
     return GetDirectoryPathA(path);
 }

#else
 inline bool  SearchFilePath(const wchar_t* filename, std::wstring& result)
 {
     return SearchFilePathW(filename, result);
 }

 inline std::wstring  RemoveDirectoryPath(const std::wstring& path)
 {
     return RemoveDirectoryPathW(path);
 }

 inline std::wstring GetDirectoryPath(const wchar_t* path)
 {
     return GetDirectoryPathW(path);
}
#endif//defined(UNICODE) || defined(_UNICODE)
