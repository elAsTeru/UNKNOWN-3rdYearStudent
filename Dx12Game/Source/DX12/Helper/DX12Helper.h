#pragma once
#include <d3d12.h>
#include <string>
#include <vector>

namespace MyDX::Helper
{
	/// <summary> リザルトをチェックし、失敗ならfalseを返却 : デバッグ時にはerrBlobよりデバッグ出力を行いそのままクラッシュする </summary>
	extern bool CheckResult(HRESULT& _Result, ID3DBlob* _ErrBlob = nullptr);

	/// <summary> ガウシアンぼかしの強さを取得 </summary>
	std::vector<float> GetGaussianWeight(size_t _Count, float _S);

	/// <summary> アライメントの値を取得 </summary>
	uint32_t AligmentedValue(uint32_t _Size, uint32_t _Alignment);
}