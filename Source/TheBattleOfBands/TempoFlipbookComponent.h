// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbookComponent.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "TempoFlipbookComponent.generated.h"

class AMusician;

UENUM()
enum EAnimState
{
	Idle,
	StartRun,
	Running,
	Stopping,
	Jumping,
	Falling,
	Landing
};

USTRUCT()
struct FTempoFlipbook
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UPaperFlipbook* Flipbook;
	UPROPERTY(EditAnywhere)
	bool bIsTempoAnimation = true;
	UPROPERTY(EditAnywhere)
	int32 CatchUpLength = 2;
};

/**
 Flipbook Component that inherits PaperZD's Flipbook component and has the option to progress to the Quartz Clock
 */
UCLASS(ClassGroup=(Tempo), meta=(BlueprintSpawnableComponent))
class THEBATTLEOFBANDS_API UTempoFlipbookComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()
	
public:
	UTempoFlipbookComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetAnimState(EAnimState NewAnimState);
	UPROPERTY(EditAnywhere)
	FOnQuartzMetronomeEventBP OnQuartzMetronomeEvent;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook IdleFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook StartRunFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook RunningFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook StoppingFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook JumpRunningFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook FallRunningFlipbook;
	UPROPERTY(EditAnywhere, Category = "Animation")
	FTempoFlipbook LandRunningFlipbook;
	
private:
	UFUNCTION(BlueprintCallable)
	void OnClockTick(FName ClockName, EQuartzCommandQuantization QuantizationType, int NumBars,
		int Beat, float BeatFraction);
	
	UPROPERTY()
	UQuartzClockHandle* TempoClock;
	int32 TickIndex = 0;
	int32 PrevBar = 0;
	int32 FrameIndex = 0;
	int32 PrevFrame = 0;
	int32 CatchUpTicks = 0;
	
	UPROPERTY(EditAnywhere)
	bool bIsStateful = true;
	UPROPERTY()
	AMusician* OwnerMusician;
	
	void HandleAnimationState();
	EAnimState CurrentAnimState = Idle;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float StopTransitionSpeed = .5f;
	
	FTempoFlipbook CurrentTempoFlipbook = IdleFlipbook;
	TMap<EAnimState, FTempoFlipbook> StateFlipbooks;
	FTimerHandle FlipbookTimer;
	void AdvancePlayRateFlipbook();
};
