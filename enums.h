#ifndef ENUMS_H
#define ENUMS_H

#include <QString>
#include <map>

namespace enums {
    enum ActionCost{extra, action, bonusAction, reaction, spellSlot, weaponAttack};
    enum damageSaveSuccess{none, full, half};
    enum aoeEffectType{sphere, cone, line, cube};

    enum abilityScoreType {diffrent, strength, dexterity, constitution, intelect, wisdom, charisma};

    enum spellLevel{spell0, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, spell9};
    enum lineOfSightType{clearLoS, noLoS, halfCover, threeQuartersCover};
    enum unitSize{small, medium, large};    

    class Maps
    {
    public:
        static ActionCost translateStrToActionCost(QString index);
        const static std::map<QString, ActionCost> strToCostMap;

        static damageSaveSuccess translateStrToDamageSaveSuccess(QString index);
        const static std::map<QString, damageSaveSuccess> strToDamageSaveSuccessMap;

        static aoeEffectType translateAoeEffectType(QString index);
        const static std::map<QString, aoeEffectType> strToAoeEffectTypeMap;

        static abilityScoreType translateStrToAbilityScoreType(QString index);
        const static std::map<QString, abilityScoreType> strToAbilityScoreTypeMap;
        static QString translateAbilityScoreTypeToStr(abilityScoreType index);
        const static std::map<abilityScoreType, QString> abilityScoreTypetoStrMap;
    };

}

using namespace enums;


#endif // ENUMS_H
