#include "skill.h"

Skill::Skill(const std::string& name, const std::string& description, int mpCost, SkillType type, int baseDamage, const std::vector<int>& hits, ChainData* chainData)
    : name(name), description(description), mpCost(mpCost), type(type), baseDamage(baseDamage), hits(hits), chainData(chainData) {
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

StealResult Skill::useSteal(Character& user, Character& target) {
    if (target.getInventory().getItems().empty()) {
        return StealResult::NoItems;
    }
}