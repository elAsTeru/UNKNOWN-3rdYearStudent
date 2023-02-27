#pragma once
#include <d3d12.h>
#include <string>
#include <vector>


/// <summary> リザルトをチェックし、失敗ならfalseを返却 : デバッグ時にはerrBlobよりデバッグ出力を行いそのままクラッシュする </summary>
extern bool CheckResult(HRESULT& _Result, ID3DBlob* _ErrBlob = nullptr);

std::vector<float> GetGaussianWeight(size_t _Count, float _S);

uint32_t AligmentedValue(uint32_t _Size, uint32_t _Alignment);