#pragma once
#include <stdexcept>

enum class Tag : unsigned char
{
	Begin,				// Tagの開始マーカー(これより前に定義禁止)

	Untagged = Begin,
	Player,
	Enemy,
	Bullet,
	BackGround,
	Ui,
	//Camera,
	//Effect,

	End,
};

// 前置インクリメント
Tag& operator ++(Tag& rhs);