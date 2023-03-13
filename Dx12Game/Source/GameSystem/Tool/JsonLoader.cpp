#include "JsonLoader.h"
#include <Rapidjson/istreamwrapper.h>
#include <fstream>
#include <Rapidjson/error/en.h>
#include <iostream>

namespace Sys::Tool
{
    rapidjson::Document LoadJson(const char* _Path)
    {
        // Json�t�@�C���ǂݍ���
        std::ifstream ifs(_Path);
        rapidjson::IStreamWrapper isw(ifs);
        // �t�@�C�����
        rapidjson::Document doc;
        doc.ParseStream(isw);
        // �`�F�b�N
        if (doc.HasParseError())
        {
            std::cout << "error offset:" << doc.GetErrorOffset() << std::endl;
            std::cout << "error pase:" << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
        }
        // �t�@�C���N���[�Y
        ifs.close();
        return doc;
    }
}