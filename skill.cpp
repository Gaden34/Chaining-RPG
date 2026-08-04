#include <iostream>
#include "skill.h"
#include "character.h"
#include "item.h"

Skill::Skill(const std::string& name, const std::string& description, int mpCost, SkillType type, int baseDamage, const std::vector<int>& hits)
    : name(name), description(description), mpCost(mpCost), type(type), baseDamage(baseDamage), hits(hits) {
}


std::vector<int> Skill::getHits() const {
    return hits;
}

SkillType Skill::getSkillTypeFromString(const std::string& type) {
    if (type == "Attack") return SkillType::Attack;
    if (type == "Magic") return SkillType::Magic;
    if (type == "Heal") return SkillType::Heal;
    if (type == "Buff") return SkillType::Buff;
    if (type == "Debuff") return SkillType::Debuff;
    if (type == "Steal") return SkillType::Steal;

    return SkillType::Attack;

}

StealResult Skill::useSteal(Character& user, Character& target, std::mt19937& rng) {
    const auto& targetItems = target.getInventory().getItems();
    if (targetItems.empty()) {
        return StealResult{StealResult::Result::NoItems, ItemID::Invalid };
    }

    std::uniform_real_distribution<float> dist(0, 1);
    for (const auto& slot : targetItems) {
        const ItemID candidateId = slot.itemID;
        const ItemData* itemData = ItemDatabase::getItemByID(candidateId);
		if (!itemData) {
			continue;
		}
		float chance = itemData->stealChance;
        float roll = dist(rng);
		std::cout << itemData->name << " steal chance: " << chance << ", roll: " << roll << std::endl;
		if (roll < chance) {
            user.getInventory().addItem(candidateId, 1);
            target.getInventory().removeItem(candidateId, 1);
            return StealResult{StealResult::Result::Success, candidateId};
		}
    }

    return StealResult{StealResult::Result::Failed, ItemID::Invalid };
}
