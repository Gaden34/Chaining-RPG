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

struct ItemEffectEvent {
    ItemEventKind kind = ItemEventKind::Heal;
    ItemEventTarget affected = ItemEventTarget::User;
    Attribute attribute = Attribute::None;
    StatusEffect status = StatusEffect::None;
    int requestedAmount = 0;
    int appliedAmount = 0;
};

struct ItemUseResult {
    bool success = false;
    std::string failureReason;
    std::vector<ItemEffectEvent> effectEvents;
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
    static ItemUseResult useItem(const ItemData& item, Character& user, Character& target);
    static void handleHealingItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result);
    static void handleStatusHealItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result);
    static void handleBuffItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result);
    static void handleDamageItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result);
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