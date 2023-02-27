#pragma once
#include <string>
#include <vector>

bool GetPaths(const std::string& _DirPath, std::vector<std::string>& _FileNames);

/// <summary> ファイルの識別子が望む識別子(_Id)か判定 </summary>
bool CheckId(const std::string& _FileName, const std::string& _Id);

bool CreateEnumClassHeader(const std::string& _FileName, const std::string& _OutDirPath, const std::vector<std::string>& _FileNames);