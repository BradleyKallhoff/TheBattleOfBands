// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "TempoLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class THEBATTLEOFBANDS_API ATempoLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	ATempoLevelScript();
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsWaltz() const {return bIsWaltz;}
	
private:
	UPROPERTY(EditAnywhere, Category="Music")
	int32 LevelBeatsPerMinute = 120;
	UPROPERTY(EditAnywhere, Category="Music")
	bool bIsWaltz = false;
	UPROPERTY(EditAnywhere, Category="Music")
	USoundBase* LevelSong;
	UPROPERTY(EditAnywhere, Category="Music")
	bool bMetronomeOn = false;
	UPROPERTY()
	UQuartzClockHandle* TempoClock;
	UPROPERTY(EditAnywhere, Category="Music")
	USoundBase* HighBeep;
	UPROPERTY()
	UAudioComponent* HighBeepSound;
	UPROPERTY(EditAnywhere, Category="Music")
	USoundBase* LowBeep;
	UPROPERTY()
	UAudioComponent* LowBeepSound;
	UPROPERTY()
	UAudioComponent* LevelSongSound;

	// UPROPERTY(EditAnywhere)
	// ATempoLevelSettings* TempoLevelSettings;
	
	UFUNCTION()
	void CreateMetronomeSounds(FName ClockName, EQuartzCommandQuantization QuatizationType, int NumBars,
		int Beat, float BeatFraction);
};
