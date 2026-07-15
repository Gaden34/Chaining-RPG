#pragma once

enum class ItemType {
    Heal,
    StatusHeal,
    Buff,
    Damage
}

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