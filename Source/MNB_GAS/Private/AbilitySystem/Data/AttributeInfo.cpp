// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool blogNotFound) const
{
    // 类中存储的infomation中遍历
    for (const FAuraAttributeInfo& Info : AttributeInfomation)
    {
        // 如果匹配到了需求Tag,Excatly 需要完全匹配
        if (Info.AttributeTag.MatchesTagExact(AttributeTag))
        {
            return Info;
        }
    }

    if (blogNotFound)
    {
        UE_LOG(LogTemp, Error, 
            TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]"), 
            *AttributeTag.ToString(), *GetNameSafe(this));

    }

    return FAuraAttributeInfo();
}
