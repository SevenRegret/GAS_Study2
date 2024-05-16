// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"
#include "MNB_GAS/MNB_GASLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool blogNotFound) const
{
    // ���д洢��infomation�б���
    for (const FAuraAttributeInfo& Info : AttributeInfomation)
    {
        // ���ƥ�䵽������Tag,Excatly ��Ҫ��ȫƥ��
        if (Info.AttributeTag.MatchesTagExact(AttributeTag))
        {
            return Info;
        }
    }

    if (blogNotFound)
    {
        UE_LOG(LogAura, Error, 
            TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]"), 
            *AttributeTag.ToString(), *GetNameSafe(this));

    }

    return FAuraAttributeInfo();
}
