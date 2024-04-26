#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameplayEffectAggregator.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"


// 使用一个结构体保存数据，（不需要暴露蓝图，因此不使用UE特性的前缀f、u等
struct AuraDamageStatics
{
    // 属性捕获帮助宏
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(LightingResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicsResistance);
    
    TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

    AuraDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
        // 护甲穿透是攻击者的来计算
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        // 暴击相关
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightingResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicsResistance, Target, false);

        const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_Armor, ArmorDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_BlockChance, BlockChanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);

        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Fire, FireResistanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Lighting, LightingResistanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, ArcaneResistanceDef);
        TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Physics, PhysicsResistanceDef);
    }
};


static const AuraDamageStatics& DamageStatics()
{
    static AuraDamageStatics DStatics;

    return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    // 这些属性是通过基础计算的来的
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
    RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().LightingResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().PhysicsResistanceDef);

}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OoutExecutionOutput) const
{
    // 1. 拿到目标和源的ASC
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();


    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

    // 2.拿到owningSpec
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // 从Magnititude获取伤害值
    float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;

        checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag),
            TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());

        const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];

        // 弹体类中将Damagevalue设置进去了，在这里根据Tag拿到
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        DamageTypeValue *= (100.f - Resistance) / 100.f;

        Damage += DamageTypeValue;

    }
    /// 格挡与护甲修正


    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().BlockChanceDef, 
        EvaluationParameters,
        TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    // 格挡成功，伤害减半
    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    if (bBlocked) Damage = Damage / 2.f;


    // 拿到GeContext（从Spec, 调用自定义的AS库，设置当前的GEContexthandle的格挡判断值
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);


    /// 护甲修


    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().ArmorDef,
        EvaluationParameters,
        TargetArmor);
    TargetArmor = FMath::Max<float>(TargetArmor, 0.f);


    float SourceArmorPenetration = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().ArmorPenetrationDef,
        EvaluationParameters,
        SourceArmorPenetration);
    SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

    // source Avatar就是一个worldcontext
    UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
    // 从CharacterClassInfo中找到对应属性的系数曲线
    FRealCurve* ArmorPenetrationCoCurve = 
        CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());

    // 通过战斗接口拿到角色等级, 在等级映射拿到系数
    const float ArmorPenetrationCoefficient = 
        ArmorPenetrationCoCurve->Eval(SourceCombatInterface->GetPlayerLevel());

    // 计算最终得出的护甲值
    const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

    // 护甲计算曲线（实际有效护甲）
    FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());

    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    // 被护甲影响的Damage（使用系数曲线的值
    Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


    /// 暴击修正

    // 暴击概率
    float SourceCriticalHitChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().CriticalHitChanceDef,
        EvaluationParameters,
        SourceCriticalHitChance);
    SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

    float TargetCriticalHitResistance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().CriticalHitResistanceDef,
        EvaluationParameters,
        TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

    float SourceCriticalHitDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().CriticalHitDamageDef,
        EvaluationParameters,
        SourceCriticalHitDamage);
    SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);


    const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
    const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

    const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
    const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

    UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

    Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

    // 执行输出
    // 选择计算类型和值
    const FGameplayModifierEvaluatedData EvaluetedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    // 特定结构初始化
    // 需要一个 上面类型的结构
    OoutExecutionOutput.AddOutputModifier(EvaluetedData);
}