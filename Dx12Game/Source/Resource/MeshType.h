#pragma once

namespace Res
{
	enum class MeshType : unsigned char
	{
		Begin,

		// Basic
		Board = Begin,
		Sphere,
		Cube,

		// Player
		Player,
		Bullet,

		// Enemy
		Rander,			// 円形穴あき四角
		Arrow,			// 矢印
		Reverser,		// 反転敵
		Tail,			// 反転敵やミサイルの尻部分
		Tracker,		// 矩形穴あき四角直線追跡敵、シールド
		CrackShield,	// シールドのひびで使用
		Missile,		// 滑らかに追跡する弾

		// Other
		GeoBox,
		GeoTriangle,
		Doughnut,

		// Test
		Teapot,

		End
	};
}