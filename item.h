#pragma once
#include <string>
#include <vector>

class Character;

enum class ItemType {
    Heal,
    StatusHeal,
    Buff,
    Damage
};

enum class Attribute { None = 0, HP, MP };

enum class StatusEffect { None = 0, Poison, Paralysis };

enum class ItemEventKind {
    Heal,
    StatusHeal,
    Buff,
    Damage
};

enum class ItemEventTarget {
    User,
    Target
};

struct ItemEffect {
    Attribute targetAttribute = Attribute::None;
    StatusEffect curesStatus = StatusEffect::None;
    StatusEffect inflictStatus = StatusEffect::None;
    int amount = 0;
    int damagePower = 0;
};


struct ItemData {
    int id = -1;
    std::string name;
    std::string description;

    int maxStackSize = 1;
    bool isConsumable = true;
    float stealChance = 0.0f;

    ItemType type = ItemType::Heal;
    //Rarity rarity;

    std::vector<ItemEffect> effects;
};

struct InventorySlot {
    int itemID;
    int quantity;
};

struct ItemUseResult {

}

class ItemDatabase {
private:
    static std::vector<ItemData> m_items;

public:
    static bool loadItems(const std::string& filePath);
    static const ItemData* getItemByID(int id);
    static const std::vector<ItemData>& getAllItems();
    static void clear();
};

class ItemSystem {
private:
    static const ItemData* getItemFromID(int targetID);

public:
    static bool useItem(const ItemData& item, Character& user, Character& target);
    static void handleHealingItem(const ItemEffect& effect, Character& user, Character& target);
    static void handleStatusHealItem(const ItemEffect& effect, Character& user, Character& target);
    static void handleBuffItem(const ItemEffect& effect, Character& user, Character& target);
    static void handleDamageItem(const ItemEffect& effect, Character& user, Character& target);
};

class Inventory {
private:
    std::vector<InventorySlot> slots;

public:
	bool addItem(int itemID, int amount = 1);
	bool removeItem(int itemID, int amount = 1);
    int getQuantity(int itemID) const;
    const std::vector<InventorySlot>& getItems() const { return slots; }
};