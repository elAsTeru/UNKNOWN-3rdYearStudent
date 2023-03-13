#include "JsonLoader.h"
#include <Rapidjson/istreamwrapper.h>
#include <fstream>
#include <Rapidjson/error/en.h>
#include <iostream>

namespace Sys::Tool
{
    rapidjson::Document LoadJson(const char* _Path)
    {
        // Jsonファイル読み込み
        std::ifstream ifs(_Path);
        rapidjson::IStreamWrapper isw(ifs);
        // ファイル解析
        rapidjson::Document doc;
        doc.ParseStream(isw);
        // チェック
        if (doc.HasParseError())
        {
            std::cout << "error offset:" << doc.GetErrorOffset() << std::endl;
            std::cout << "error pase:" << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
        }
        // ファイルクローズ
        ifs.close();
        return doc;
    }
}