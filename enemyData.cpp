#include "enemyData.h"

EnemyData knight{
	"Knight",
	63,
	10,
	8,
	5,
	25,
	100.f,
	"assets/enemyknight.png",
	{
		{ ItemID::ThrowingKnife, 1 },
		{ ItemID::Potion, 1 }  
	}
};

EnemyData bat{
	"Bat",
	52,
	6,
	6,
	2,
	18,
	150.f,
	"assets/batSprite.png",
	{
		{ ItemID::BatFang, 1 },
		{ ItemID::Ether, 1 }
	}
};

EnemyData mandaro{
	"Mandaro",
	65,
	12,
	10,
	8,
	35,
	120.f,
	"assets/MandaroSprite.png",
	{
		{ ItemID::MandaroCrown, 2 },
		{ ItemID::Ether, 1 }
	}
};