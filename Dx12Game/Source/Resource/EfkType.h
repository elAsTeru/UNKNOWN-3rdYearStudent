#pragma once

namespace Res
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
}