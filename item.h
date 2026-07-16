#pragma once

enum class ItemType {
    Heal,
    StatusHeal,
    Buff,
    Damage
}

enum class Attribute { HP, MP }

enum class StatusEffect { Poison, Paralysis }

struct ItemData {
    int id;
    std::string name;
    std::string description;

    int maxStackSize;
    bool isConsumable;

    ItemType type;
    Rarity rarity;
}

class Item {
private:


public:
}