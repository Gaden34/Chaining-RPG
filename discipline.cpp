#include "discipline.h"
#include "disciplines.h"

Discipline::Discipline(std::string n, int health, int mp, int attack, int magAttack, StatGrowth growth) : name(n), baseHealth(health), baseMp(mp), baseAttack(attack), baseMagAttack(magAttack), statGrowth(growth) {}

std::string Discipline::getName() {
	return name;
}

Discipline& Disciplines::getDisciplineFromID(DisciplineID id) {
	switch (id) {
	case DisciplineID::Mage:
		return Disciplines::Mage;

	case DisciplineID::Thief:
		return Disciplines::Thief;

	case DisciplineID::Combatant:
		return Disciplines::Combatant;

	}
}