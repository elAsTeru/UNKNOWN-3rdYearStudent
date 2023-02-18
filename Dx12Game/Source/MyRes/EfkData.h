#pragma once
#include <unordered_map>
#include "EfkType.h"

namespace MyRes::EfkData
{
	// ファイルパス
	std::unordered_map<EfkType, const wchar_t*> pathMap =
	{
		{EfkType::Spawn,L"Resource/Effekseer/Efk/spawn.efk"},
		{EfkType::Track,L"Resource/Effekseer/track.efk"},
		{EfkType::Explosion,L"Resource/Effekseer/Efk/explosion3.efk"},
		{EfkType::Explosion2,L"Resource/Effekseer/Efk/explosion2.efk"},
		{EfkType::PlayerEliminate,L"Resource/Effekseer/Efk/explosion.efk"},
		{EfkType::Hit,L"Resource/Effekseer/Efk/hit.efk"},
		{EfkType::Laser,L"Resource/Effekseer/Efk/laser.efk"},
		{EfkType::LaserEnd,L"Resource/Effekseer/Efk/laserEnd.efk"}
	};
}