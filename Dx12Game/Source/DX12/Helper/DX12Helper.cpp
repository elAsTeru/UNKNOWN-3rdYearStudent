#include "DX12Helper.h"
#include <assert.h>

namespace MyDX::Helper
{
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
}