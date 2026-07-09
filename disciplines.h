#pragma once
#include "discipline.h"

namespace Disciplines {
	Discipline& getDisciplineFromID(DisciplineID id);

	inline Discipline Mage{
		"Mage",
		27,
		20,
		10,
		15,
		{ 3, 2, 1, 2}
		
	};

	inline Discipline Thief{
		"Thief",
		29,
		10,
		12,
		8,
		{ 4, 2, 2, 1 }
	};

	inline Discipline Combatant{
		"Combatant",
		32,
		8,
		15,
		5,
		{ 5, 1, 3, 1 }
	};
}