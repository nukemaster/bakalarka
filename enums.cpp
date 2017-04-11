#include "enums.h"

const std::map<QString, ActionCost> Maps::strToCostMap = {
    {"action", action},
    {"bonusAction", bonusAction},
    {"reaction", reaction},
    {"spellSlot", spellSlot},
    {"weaponAttack", weaponAttack},
    {"extra", extra}
 };

const std::map<QString, damageSaveSuccess> Maps::strToDamageSaveSuccessMap = {
    {"none", none},
    {"full", full},
    {"half", half}
 };

const std::map<QString, aoeEffectType> Maps::strToAoeEffectTypeMap = {
    {"sphere", sphere},
    {"cone", cone},
    {"line", line},
    {"cube", cube}
 };

const std::map<QString, abilityScoreType> Maps::strToAbilityScoreTypeMap = {
    {"strength", strength},
    {"constitution", constitution},
    {"dexterity", dexterity},
    {"intelect", intelect},
    {"wisdom", wisdom},
    {"charisma", charisma}
 };

const std::map<abilityScoreType, QString> Maps::abilityScoreTypetoStrMap = {
    {strength, "strength"},
    {constitution, "constitution"},
    {dexterity, "dexterity"},
    {intelect, "intelect"},
    {wisdom, "wisdom"},
    {charisma, "charisma"}
};

ActionCost Maps::translateStrToActionCost(QString index)
{
    return (strToCostMap.count(index))?strToCostMap.at(index) : extra;
}

damageSaveSuccess Maps::translateStrToDamageSaveSuccess(QString index)
{
    return (strToDamageSaveSuccessMap.count(index))?strToDamageSaveSuccessMap.at(index) : half;
}

aoeEffectType Maps::translateAoeEffectType(QString index)
{
    return (strToAoeEffectTypeMap.count(index))?strToAoeEffectTypeMap.at(index) : sphere;
}

abilityScoreType Maps::translateStrToAbilityScoreType(QString index)
{
    return (strToAbilityScoreTypeMap.count(index))? strToAbilityScoreTypeMap.at(index) : diffrent;
}

QString Maps::translateAbilityScoreTypeToStr(abilityScoreType index)
{
    return (abilityScoreTypetoStrMap.count(index))? abilityScoreTypetoStrMap.at(index) : "diffrent";
}
