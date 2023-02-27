#include "Dir.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Helper/StringHelper.h"

// �f�B���N�g���p�X�̍Ō� / �ŏI���Ȃ��ꍇ \\ �ɂȂ��Ă��܂��̂őΉ������Ă����ƈ��S
bool GetPaths(const std::string& _DirPath, std::vector<std::string>& _FileNames)
{
	std::filesystem::directory_iterator itr(_DirPath), end;
	std::error_code err;

	for (; itr != end && !err; itr.increment(err)) {
		const std::filesystem::directory_entry entry = *itr;

		_FileNames.push_back(entry.path().string());
		printf("%s\n", _FileNames.back().c_str());
	}

	/* �G���[���� */
	if (err) {
		std::cout << err.value() << std::endl;
		std::cout << err.message() << std::endl;
		return false;
	}
	return true;
}

bool CheckId(const std::string& _FileName, const std::string& _Id)
{
	bool isId = false;	// ���ʎq��?
	std::string id;		// ���ʎq

	for (auto itr = _FileName.begin();itr != _FileName.end(); ++itr)
	{
		// ���ʎq�Ȃ�L�^����
		if (isId)
		{
			id += *itr;
		}
		// '.'�������环�ʎq�t���O��true�ɂ���
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
	// �f�B���N�g�����̃t�@�C������Ȃ�
	if (_FileNames.size() == 0)
	{
		return false;
	}

	std::ofstream outputfile(_OutDirPath + _FileName + ".h");

	// �����o���������i�[����
	std::string outData{};

	outData += "#pragma once\n\n";	// #pragma once
	outData += "namespace Res\n";	// namespace Res
	outData += "{\n";				// begin namespace Res

	outData += "\tenum class " + _FileName + ":unsigned char\n";	// enum class name
	outData += "\t{\n";												// begin enum class

	// �񋓊J�n
	outData += "\t\tBegin = 0,\n\n";

	for (auto filePath : _FileNames)
	{
		std::filesystem::path path = filePath;				// �t�@�C�������擾���邽�߂Ƀt�@�C���V�X�e���Ńp�X���L�^
		std::string tempFileName = path.filename().string();	// �t�@�C�������擾
		std::string fileName{};

		// ���ʎq����菜���A'.'�������烋�[�v���I��
		for (auto chara : tempFileName)
		{
			if (chara == '.')
			{
				break;
			}
			fileName += chara;
		}

		// �擪�̕������������Ȃ�啶���ɕϊ�
		System::Helper::ToUpper(fileName[0]);
		
		outData += "\t\t" + fileName;
		// �擪�v�f�̏ꍇ
		if (filePath == _FileNames[0])
		{
			outData += "=Begin";
		}
		outData += ",\n";
	}

	outData += "\n\t\tEnd\n";
	// �񋓏I��


	outData += "\t}; \n";		// end enum class
	outData += "} \n";			// end namespace Res

	// �w�b�_�t�@�C�������o��
	outputfile << outData;
	outputfile.close();

	return true;
}