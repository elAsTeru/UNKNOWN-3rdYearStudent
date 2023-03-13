#pragma once
#include <unordered_map>
#include <vector>
#include "Tool/DXTK12Font.h"
#include "DirectXTK12/SimpleMath.h"

// JSONファイルを読み込むときにSJIS→UTF8の変換が起きるときに、
// "Ⓐ"のように文字化けする文字があるので、JSONを今回は不使用。

using DirectX::SimpleMath::Vector4;

static constexpr Vector4 Orange(1, 0.65f, 0, 1.0f);
static constexpr Vector4 TransOrange(1, 0.65f, 0, 0.1f);
static constexpr Vector4 Purple(0.20f, 0.15f, 0.25f, 1.0f);

namespace Res
{
	//enum class ColorType
	//{
	//	Orange,
	//	Purple
	//};

	//std::unordered_map<ColorType, Vector4> colors
	//{
	//	{ColorType::Orange, Vector4(1.0f, 0.65f, 0, 1.0f)},
	//	{ColorType::Purple, Vector4(0.20f, 0.15f, 0.25f, 1.0f)}
	//};

	enum class FontType
	{
		Title,
		Select,
		Result
	};

	static std::unordered_map<FontType, std::vector<MyDX::FontData>> fontData =
	{
		{
			FontType::Title, 
			{
				MyDX::FontData(L"NKNOWN", {290, 410}, {1, 0.65f, 0, 1}, 0, {0, 0}, 1),
				MyDX::FontData(L"Ⓐ プレイ", {330, 580}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.7f)
			}
		},
		{
			FontType::Select,
			{
				MyDX::FontData(L"ステージ選択", {650, 210}, { 0.20f, 0.15f, 0.25f, 1.0f }, 0, {0, 0}, 1),
				MyDX::FontData(L"ステージ選択", {645, 205}, {1, 0.65f, 0, 0.1f}, 0, {0, 0}, 1.02f),
				MyDX::FontData(L"ステージ 1", {350, 670}, { 0.20f, 0.15f, 0.25f, 1.0f }, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"ステージ 2", {830, 670}, { 0.20f, 0.15f, 0.25f, 1.0f }, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"ステージ 3", {1310, 670}, { 0.20f, 0.15f, 0.25f, 1.0f }, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"ステージ 1", {344, 670}, { 1, 0.65f, 0, 0.1f }, 0, {0, 0}, 0.52f),
				MyDX::FontData(L"ステージ 2", {824, 670}, { 1, 0.65f, 0, 0.1f }, 0, {0, 0}, 0.52f),
				MyDX::FontData(L"ステージ 3", {1304, 670}, { 1, 0.65f, 0, 0.1f }, 0, {0, 0}, 0.52f),
				MyDX::FontData(L"UNKNOWN", {140, 2}, { 1, 0.65f, 0, 0.1f }, 0, {0, 0}, 1),
				MyDX::FontData(L"⇦ 選択 ⇨　Ⓐ 決定", {140, 970}, { 1, 0.65f, 0, 0.1f }, 0, {0, 0}, 0.5f),
			}
		},
		{
			FontType::Result,
			{
				MyDX::FontData(L"RESULT", {300, 185}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.8f),
				MyDX::FontData(L"RESULT", {300, 180}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.8f),

				MyDX::FontData(L"Eliminate", {450, 295}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"Eliminate", {450, 290}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"ElimScore", {540, 365}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"ElimScore", {540, 360}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"Life", {450, 435}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"Life", {450, 430}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"LifeScore", {540, 505}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"LifeScore", {540, 500}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"Stage", {450, 575}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"Stage", {450, 570}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"StageScore", {540, 645}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"StageScore", {540, 640}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"Boss", {450, 715}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"Boss", {450, 710}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"BossScore", {540, 785}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.5f),
				MyDX::FontData(L"BossScore", {540, 780}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.5f),

				MyDX::FontData(L"Total Score", {950, 385}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.9f),
				MyDX::FontData(L"Total Score", {950, 380}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.9f),

				MyDX::FontData(L"TotalScore", {1050, 485}, {0.05f, 0.05f, 0.05f, 1}, 0, {0, 0}, 0.9f),
				MyDX::FontData(L"TotalScore", {1050, 480}, {1, 0.65f, 0, 1}, 0, {0, 0}, 0.9f),

				MyDX::FontData(L"Ⓐ 戻る", {140, 970}, {1, 0.65f, 0, 0.1f}, 0, {0, 0}, 0.5f),
			}
		}
	};
}