#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameplayEffectAggregator.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"


// ʹ��һ���ṹ�屣�����ݣ�������Ҫ��¶��ͼ����˲�ʹ��UE���Ե�ǰ׺f��u��
struct AuraDamageStatics
{
    // ���Բ��������
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
        // ���״�͸�ǹ����ߵ�������
        DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
        // �������
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
    // ��Щ������ͨ���������������
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
    // 1. �õ�Ŀ���Դ��ASC
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();


    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

    // 2.�õ�owningSpec
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluationParameters;
    
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // ��Magnititude��ȡ�˺�ֵ
    float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;
        const FGameplayTag ResistanceTag = Pair.Value;

        checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag),
            TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());

        const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];

        // �������н�Damagevalue���ý�ȥ�ˣ����������Tag�õ�
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

        DamageTypeValue *= (100.f - Resistance) / 100.f;

        Damage += DamageTypeValue;

    }
    /// ���뻤������


    float TargetBlockChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().BlockChanceDef, 
        EvaluationParameters,
        TargetBlockChance);
    TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

    // �񵲳ɹ����˺�����
    const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
    if (bBlocked) Damage = Damage / 2.f;


    // �õ�GeContext����Spec, �����Զ����AS�⣬���õ�ǰ��GEContexthandle�ĸ��ж�ֵ
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);


    /// ������


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

    // source Avatar����һ��worldcontext
    UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
    // ��CharacterClassInfo���ҵ���Ӧ���Ե�ϵ������
    FRealCurve* ArmorPenetrationCoCurve = 
        CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());

    // ͨ��ս���ӿ��õ���ɫ�ȼ�, �ڵȼ�ӳ���õ�ϵ��
    const float ArmorPenetrationCoefficient = 
        ArmorPenetrationCoCurve->Eval(SourceCombatInterface->GetPlayerLevel());

    // �������յó��Ļ���ֵ
    const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

    // ���׼������ߣ�ʵ����Ч���ף�
    FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());

    const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
    // ������Ӱ���Damage��ʹ��ϵ�����ߵ�ֵ
    Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


    /// ��������

    // ��������
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

    // ִ�����
    // ѡ��������ͺ�ֵ
    const FGameplayModifierEvaluatedData EvaluetedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
    // �ض��ṹ��ʼ��
    // ��Ҫһ�� �������͵Ľṹ
    OoutExecutionOutput.AddOutputModifier(EvaluetedData);
}