#pragma once
#include "discipline.h"

namespace Disciplines {
	Discipline& getDisciplineFromID(DisciplineID id);

	inline Discipline Mage{
		"Mage",
		27,
		10,
		15
	};

	inline Discipline Thief{
		"Thief",
		29,
		12,
		8
	};

	inline Discipline Combatant{
		"Combatant",
		32,
		15,
		5
	};
}