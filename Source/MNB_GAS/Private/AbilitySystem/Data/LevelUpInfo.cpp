// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;

	while (bSearching)
	{
		// �Ѿ��������ȼ�
		if (LevelUpInformation.Num() - 1 <= Level) return Level;

		// �ҵ���ǰ�ܾ���Ӧ���Ƕ��ټ�
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}

	return Level;
}