// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TempoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THEBATTLEOFBANDS_API ATempoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATempoGameMode();
	
	UPROPERTY(EditAnywhere, Category="Music")
	int32 BeatsPerMinute = 120;
	UPROPERTY(EditAnywhere, Category="Music")
	bool bIsWaltz = false;
	
	void PawnKilled(AActor* Victim);
};
