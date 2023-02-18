#pragma once
namespace MyRes
{
	enum class EfkType : unsigned char
	{
		Begin,

		Spawn = Begin,
		Track,
		Explosion,
		Explosion2,
		PlayerEliminate,
		Hit,
		Laser,
		LaserEnd,

		End
	};

	// 前置インクリメント
	EfkType& operator ++(EfkType& rhs);
}