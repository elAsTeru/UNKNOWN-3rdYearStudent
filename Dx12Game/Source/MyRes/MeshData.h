#pragma once
#include <unordered_map>
#include "MeshType.h"

namespace MyRes::MeshData
{
	// ファイルパス
	std::unordered_map<MeshType, const wchar_t*> pathMap =
	{
		{MeshType::Board,L"Resource/model/board.obj"},
		{MeshType::Sphere,L"Resource/model/sphere.obj"},
		{MeshType::Cube,L"Resource/model/cube.obj"},

		{MeshType::Player,L"Resource/model/fighter.obj"},
		{MeshType::Bullet,L"Resource/model/bullet.obj"},
		
		{MeshType::Rander,L"Resource/model/randomCube.obj"},
		{MeshType::Arrow,L"Resource/model/randomArrow.obj"},
		{MeshType::Reverser,L"Resource/model/reverserHead.obj"},
		{MeshType::Tail,L"Resource/model/reverserTail.obj"},
		{MeshType::Tracker,L"Resource/model/cubeHole.obj"},
		{MeshType::ShieldCrack,L"Resource/model/crack.obj"},
		{MeshType::Missile,L"Resource/model/shot.obj"},

		{MeshType::GeoBox,L"Resource/model/geoBox.obj"},
		{MeshType::GeoTriangle,L"Resource/model/GeoTriangle.obj"},
		{MeshType::Doughnut,L"Resource/model/Doughnut.obj"},

		{MeshType::Teapot,L"Resource/model/teapot.obj"}
	};
}