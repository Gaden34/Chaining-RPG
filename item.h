#pragma once
#include <vector>

class Character;

enum class ItemType {
    Heal,
    StatusHeal,
    Buff,
    Damage
}

enum class Attribute { None = 0, HP, MP }

enum class StatusEffect { None = 0, Poison, Paralysis }

struct ItemEffect {
    Attribute targetAttribute = Attribute::None;
    StatusEffect cureStatus = StatusEffect::None;
    StatusEffect inflictStatus = StatusEffect::None;

}

struct ItemData {
    int id;
    std::string name;
    std::string description;

    int maxStackSize;
    bool isConsumable;

    ItemType type;
    //Rarity rarity;

    std::vector<ItemEffect> effects;
}

class ItemSystem {
private:


public:

static bool useItem(const ItemData& item, Character& user, Character& target);
}