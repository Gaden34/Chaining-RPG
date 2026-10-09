#include "item.h"
#include "character.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

std::string toLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

ItemType parseItemType(const std::string& typeStr) {
    const std::string key = toLowerCopy(typeStr);
    if (key == "heal") {
        return ItemType::Heal;
    }
    if (key == "statusheal" || key == "status_heal") {
        return ItemType::StatusHeal;
    }
    if (key == "buff") {
        return ItemType::Buff;
    }
    if (key == "damage") {
        return ItemType::Damage;
    }
    return ItemType::Heal;
}

Attribute parseAttribute(const std::string& attributeStr) {
    const std::string key = toLowerCopy(attributeStr);
    if (key == "hp") {
        return Attribute::HP;
    }
    if (key == "mp") {
        return Attribute::MP;
    }
    return Attribute::None;
}

StatusEffect parseStatusEffect(const std::string& statusStr) {
    const std::string key = toLowerCopy(statusStr);
    if (key == "poison") {
        return StatusEffect::Poison;
    }
    if (key == "paralysis") {
        return StatusEffect::Paralysis;
    }
    return StatusEffect::None;
}

ItemEffect parseEffect(const json& effectJson) {
    ItemEffect effect;
    effect.targetAttribute = parseAttribute(effectJson.value("target_attribute", "None"));
    effect.curesStatus = parseStatusEffect(effectJson.value("cures_status", "None"));
    effect.inflictStatus = parseStatusEffect(effectJson.value("inflict_status", "None"));
    effect.amount = effectJson.value("amount", 0);
    effect.damagePower = effectJson.value("damage_power", 0);
    effect.hits = effectJson.value("hits", std::vector<int>{});
    effect.hitTimes = effectJson.value("hit_times", std::vector<float>{});
    return effect;
}

} // namespace

std::vector<ItemData> ItemDatabase::m_items;

bool ItemDatabase::loadItems(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open items JSON file at: " << filePath << std::endl;
        return false;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse items JSON: " << e.what() << std::endl;
        return false;
    }

    const json& itemsJson = data.contains("items") ? data["items"] : data;
    if (!itemsJson.is_array()) {
        std::cerr << "Items JSON root must be an array or contain an \"items\" array." << std::endl;
        return false;
    }

    m_items.clear();
    m_items.reserve(itemsJson.size());

    for (const auto& itemJson : itemsJson) {
        ItemData item;
        item.id = static_cast<ItemID>(itemJson.value("id", -1));
        item.name = itemJson.value("name", "Unknown Item");
        item.description = itemJson.value("description", "");
        item.animationName = itemJson.value("animation_name", "");
        item.maxStackSize = itemJson.value("max_stack_size", 1);
		item.sellValue = itemJson.value("sell_value", 0);
        item.isConsumable = itemJson.value("is_consumable", true);
        item.stealChance = itemJson.value("steal_chance", 0.0f);
        item.type = parseItemType(itemJson.value("type", "Heal"));

        if (itemJson.contains("effects") && itemJson["effects"].is_array()) {
            for (const auto& effectJson : itemJson["effects"]) {
                item.effects.push_back(parseEffect(effectJson));
            }
        }

        if (static_cast<int>(item.id) < 0) {
            std::cerr << "Skipping item with invalid id: " << item.name << std::endl;
            continue;
        }

        m_items.push_back(std::move(item));
    }
    std::cout <<"LOADED ITEMS" << std::endl;
    return true;
}

const ItemData* ItemDatabase::getItemByID(ItemID id) {
    for (const auto& item : m_items) {
        if (item.id == id) {
            return &item;
        }
    }
    return nullptr;
}

const std::vector<ItemData>& ItemDatabase::getAllItems() {
    return m_items;
}

void ItemDatabase::clear() {
    m_items.clear();
}

const ItemData* ItemSystem::getItemFromID(ItemID targetID) {
    return ItemDatabase::getItemByID(targetID);
}

ItemUseResult ItemSystem::useItem(const ItemData& item, Character& user, Character& target) {
    ItemUseResult result;

    if (item.effects.empty()) {
        result.failureReason = "Item has no effect.";
        return result;
    }

    switch (item.type) {
        case ItemType::Heal:
            for (const auto& effect : item.effects) {
                handleHealingItem(effect, user, target, result);
            }
            break;
        case ItemType::StatusHeal:
            for (const auto& effect : item.effects) {
                handleStatusHealItem(effect, user, target, result);
            }
            break;
        case ItemType::Buff:
            for (const auto& effect : item.effects) {
                handleBuffItem(effect, user, target, result);
            }
            break;
        case ItemType::Damage:
            for (const auto& effect : item.effects) {
                handleDamageItem(effect, user, target, result);
            }
            break;
        default:
            result.failureReason = "Unknown item type.";
            break;
    }

    if (result.failureReason.empty() && result.effectEvents.empty()) {
        result.failureReason = "Item had no effect.";
    }

    result.success = result.failureReason.empty();

    return result;
}

void ItemSystem::handleHealingItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result) {
    ItemEffectEvent event;
    event.kind = ItemEventKind::Heal;
    event.affected = ItemEventTarget::Target;
    event.attribute = effect.targetAttribute;
    event.requestedAmount = effect.amount;

    if (effect.targetAttribute == Attribute::HP) {
        if (target.getHp() >= target.getMaxHp()) {
            if (result.failureReason.empty()) {
                result.failureReason = target.getName() + " HP is already full.";
            }
            return;
        }
        int before = target.getHp();
        target.setHp(target.getHp() + effect.amount);
        event.appliedAmount = target.getHp() - before;
    } else if (effect.targetAttribute == Attribute::MP) {
        if (target.getMp() >= target.getMaxMp()) {
            if (result.failureReason.empty()) {
                result.failureReason = target.getName() + " MP is already full.";
            }
            return;
        }
        int before = target.getMp();
        target.setMp(target.getMp() + effect.amount);
        event.appliedAmount = target.getMp() - before;
    } else {
        return;
    }
    result.effectEvents.push_back(event);
}

void ItemSystem::handleStatusHealItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result) {
    if (effect.curesStatus != StatusEffect::None) {
        // Implement status cure logic here when Character exposes status APIs.
    }
}

void ItemSystem::handleBuffItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result) {
    // Implement buff logic here when Character exposes buff APIs.
}

void ItemSystem::handleDamageItem(const ItemEffect& effect, Character& user, Character& target, ItemUseResult& result) {
    ItemEffectEvent event;
    event.kind = ItemEventKind::Damage;
    event.affected = ItemEventTarget::Target;
    event.attribute = effect.targetAttribute;

    if (effect.targetAttribute == Attribute::HP) {
        event.requestedAmount = effect.damagePower;
        target.takeDamage(effect.damagePower);
        event.appliedAmount = effect.damagePower;
        if (effect.amount != 0)
            user.setHp(user.getHp() + effect.amount);
    } else if (effect.targetAttribute == Attribute::MP) {
        event.requestedAmount = effect.damagePower;
        target.setMp(target.getMp() - effect.damagePower);
        event.appliedAmount = effect.damagePower;
        if (effect.amount != 0)
            user.setMp(user.getMp() + effect.amount);
    } else {
        return;
    }
    result.effectEvents.push_back(event);
}

bool Inventory::addItem(ItemID itemID, int quantity) {
    if (quantity <= 0) {
        return false;
    }

    const ItemData* itemData = ItemDatabase::getItemByID(itemID);
    if (!itemData) {
        return false;
    }

    for (auto& slot : slots) {
        if (slot.itemID == itemID) {
            int newQuantity = slot.quantity + quantity;
            if (newQuantity > itemData->maxStackSize) {
                slot.quantity = itemData->maxStackSize;
                return true;
            } else {
                slot.quantity = newQuantity;
                return true;
            }
        }
    }

    if (slots.size() < 1000) {
        slots.push_back({itemID, std::min(quantity, itemData->maxStackSize)});
        return true;
    }

    return false;
}

bool Inventory::removeItem(ItemID itemID, int amount) {
    if (amount <= 0) {
        return false;
    }

    for (auto it = slots.begin(); it != slots.end(); ++it) {
        if (it->itemID == itemID) {
            if (it->quantity > amount) {
                it->quantity -= amount;
                return true;
            } else if (it->quantity == amount) {
                slots.erase(it);
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}

int Inventory::getQuantity(ItemID itemID) const {
    for (const auto& slot : slots) {
        if (slot.itemID == itemID) {
            return slot.quantity;
        }
    }
    return 0;
}
