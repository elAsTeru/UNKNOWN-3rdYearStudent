#include "DX12Helper.h"
#include <assert.h>

bool CheckResult(HRESULT& _Result, ID3DBlob* _ErrBlob)
{
	if (FAILED(_Result)) {
#ifdef _DEBUG
		if (_ErrBlob != nullptr) {
			std::string outmsg;
			outmsg.resize(_ErrBlob->GetBufferSize());
			std::copy_n(static_cast<char*>(_ErrBlob->GetBufferPointer()),
				_ErrBlob->GetBufferSize(),
				outmsg.begin());
			OutputDebugString(outmsg.c_str());
		}
		assert(SUCCEEDED(_Result));
#endif
		return false;
	}
	else {
		return true;
	}
}

std::wstring GetWideStringFromString(const std::string& _Str)
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

std::vector<float> GetGaussianWeight(size_t _Count, float _S)
{
	std::vector<float> weights(_Count);	// ウェイト配列返却用
	float x = 0.0f;
	float total = 0.0f;

	// 右半分
	for (auto& wgt : weights)
	{
		wgt = expf(-(x * x) / (2 * _S * _S));
		total += wgt;
		x += 1.0f;
	}

	// 左半分
	total = total + 2 - 1;
	// 足して１になるように調整
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}

uint32_t AligmentedValue(uint32_t _Size, uint32_t _Alignment)
{
	return (_Size + _Alignment - (_Size % _Alignment));
}