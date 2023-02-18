#pragma once
#include <d3d12.h>
#include <string>
#include <vector>

///リザルトをチェックし、ダメだったらfalseを返す
///@param result DX関数からの戻り値
///@param errBlob エラーがあるならエラーを出力
///@remarks 

/// <summary>
/// リザルトをチェックし、失敗ならfalseを返却 : デバッグ時にはerrBlobよりデバッグ出力を行いそのままクラッシュする
/// </summary>
/// <param name="result">DX関数からの戻り値</param>
/// <param name="errBlob">エラーがある場合エラー出力</param>
extern bool CheckResult(HRESULT& _Result, ID3DBlob* _ErrBlob = nullptr);


/// <summary>
/// string(マルチバイト文字列)からwstring(ワイド文字列)を返却
/// </summary>
std::wstring GetWideStringFromString(const std::string& _Str);

std::vector<float> GetGaussianWeight(size_t _Count, float _S);

uint32_t AligmentedValue(uint32_t _Size, uint32_t _Alignment);