#pragma once
#include "discipline.h"

namespace Disciplines {
	Discipline& getDisciplineFromID(DisciplineID id);

	inline Discipline Mage{
		"Mage",
		27,
		10,
		15,
		20
	};

	inline Discipline Thief{
		"Thief",
		29,
		12,
		8,
		10
	};

	inline Discipline Combatant{
		"Combatant",
		32,
		15,
		5,
		8
	};
}