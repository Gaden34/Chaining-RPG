#include "skill.h"

Skill::Skill(const std::string& name, const std::string& description, int mpCost, float cooldown, SkillType type, int baseDamage, const std::vector<int>& hits, ChainData* chainData)
    : name(name), description(description), mpCost(mpCost), cooldown(cooldown), type(type), baseDamage(baseDamage), hits(hits), chainData(chainData) {
}

void Skill::initializeChaining(float chainWindow, float damagePerChain) {
    if (!chainData) {
        chainData = new ChainData();
    }
    chainData->chainWindow = chainWindow;
    chainData->damageMultiplier = 1.0f + (damagePerChain / 100.0f);
}

void Skill::updateChainTimer(float dt) {
    if (chainData && chainData->canChain) {
        chainData->chainTimer -= dt;
        if (chainData->chainTimer <= 0.f) {
            chainData->chainTimer = 0.f;
            chainData->chainHitCount = 0;
            chainData->damageMultiplier = 1.0f;
        }
    }
}

void Skill::onHit() {
    if (chainData && chainData->canChain) {
        chainData->chainHitCount++;
        chainData->chainTimer = chainData->chainWindow;
        chainData->damageMultiplier = 1.0f + (chainData->chainHitCount * 0.25f); // 25% per chain hit
    }
}

float Skill::getDamage() const {
    if (chainData && chainData->canChain) {
        return baseDamage * chainData->damageMultiplier;
    }
    return baseDamage;
}

bool Skill::canChainIntoNextSkill() const {
    if (!chainData || !chainData->canChain) return false;
    return chainData->chainTimer > 0.f && chainData->chainHitCount > 0;
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