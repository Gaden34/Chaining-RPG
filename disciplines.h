#pragma once
#include "discipline.h"

namespace Disciplines {
	Discipline& getDisciplineFromID(DisciplineID id);

	inline Discipline Mage{
		"Mage",
		75,
		50,
		21,
		30,
		{ 3, 2, 1, 2 }
	};


	inline Discipline Thief{
		"Thief",
		80,
		35,
		28,
		18,
		{ 4, 2, 2, 1 }
	};

	inline Discipline Combatant{
		"Combatant",
		84,
		35,
		33,
		15,
		{ 5, 1, 3, 1 }
	};

}