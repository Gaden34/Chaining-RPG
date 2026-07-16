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
        item.id = itemJson.value("id", -1);
        item.name = itemJson.value("name", "Unknown Item");
        item.description = itemJson.value("description", "");
        item.maxStackSize = itemJson.value("max_stack_size", 1);
        item.isConsumable = itemJson.value("is_consumable", true);
        item.type = parseItemType(itemJson.value("type", "Heal"));

        if (itemJson.contains("effects") && itemJson["effects"].is_array()) {
            for (const auto& effectJson : itemJson["effects"]) {
                item.effects.push_back(parseEffect(effectJson));
            }
        }

        if (item.id < 0) {
            std::cerr << "Skipping item with invalid id: " << item.name << std::endl;
            continue;
        }

        m_items.push_back(std::move(item));
    }

    return true;
}

const ItemData* ItemDatabase::getItemByID(int id) {
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

const ItemData* ItemSystem::getItemFromID(int targetID) {
    return ItemDatabase::getItemByID(targetID);
}

bool ItemSystem::useItem(const ItemData& item, Character& user, Character& target) {
    (void)user;
    bool appliedAnyEffect = false;

    for (const auto& effect : item.effects) {
        switch (effect.targetAttribute) {
            case Attribute::HP:
                target.setHp(target.getHp() + effect.amount);
                appliedAnyEffect = true;
                break;

            case Attribute::MP:
                target.setMp(target.getMp() + effect.amount);
                appliedAnyEffect = true;
                break;

            case Attribute::None:
            default:
                break;
        }

        switch (effect.curesStatus) {
            case StatusEffect::Poison:
            case StatusEffect::Paralysis:
                // Status application hooks can be implemented when Character exposes status APIs.
                appliedAnyEffect = true;
                break;

            case StatusEffect::None:
            default:
                break;
        }

        switch (effect.inflictStatus) {
            case StatusEffect::Poison:
            case StatusEffect::Paralysis:
                // Status application hooks can be implemented when Character exposes status APIs.
                appliedAnyEffect = true;
                break;

            case StatusEffect::None:
            default:
                break;
        }
    }

    return appliedAnyEffect;
}