#include "item.h"
#include "character.h"

bool ItemSystem::useItem(const ItemData& data, Character& user, Character& target) {


    for (auto& effect : itemEffects) {
        switch(effect.targetAttribute) {
            case Attribute::HP:
                //target.modifyHP(effect.modifierAmount);
                break;

            case Attribute::None:
            default:
                break;
        }

        switch(effect.curesStatus) {
            case StatusEffect::Poison:
                //target.CureStatus(effect.curesStatus)
            
            case StatusEffect::None:
            default:
                break;
        }
    }
}