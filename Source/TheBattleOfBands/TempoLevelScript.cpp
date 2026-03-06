// Fill out your copyright notice in the Description page of Project Settings.


#include "TempoLevelScript.h"

#include "TempoGameMode.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ATempoLevelScript::ATempoLevelScript()
{
	PrimaryActorTick.bCanEverTick = true;
	// TempoLevelSettings = CreateDefaultSubobject<ATempoLevelSettings>("Level Settings");
}

void ATempoLevelScript::BeginPlay()
{
	Super::BeginPlay();
	
	if (ATempoGameMode* TempoGameMode = Cast<ATempoGameMode>(GetWorld()->GetAuthGameMode()))
	{
		TempoGameMode->BeatsPerMinute = LevelBeatsPerMinute;
		TempoGameMode->bIsWaltz = bIsWaltz;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Using Tempo Player Start without Temp Game Mode"));
	}
	
	UQuartzSubsystem* QuartzSubsystem = GetWorld()->GetSubsystem<UQuartzSubsystem>();
	
	FQuartzTimeSignature TimeSignature;
	if (bIsWaltz) TimeSignature.NumBeats = 3;
	TimeSignature.BeatType = EQuartzTimeSignatureQuantization::HalfNote;
	
	const FQuartzClockSettings ClockSettings = {
		TimeSignature,
		false,
	};
	
	TempoClock = QuartzSubsystem->CreateNewClock(
		this,
		TEXT("Tempo Clock"),
		ClockSettings,
		false);
	
	HighBeepSound = UGameplayStatics::CreateSound2D(this, HighBeep);
	HighBeepSound->bCanPlayMultipleInstances = true;
	LowBeepSound = UGameplayStatics::CreateSound2D(this, LowBeep);
	LowBeepSound->bCanPlayMultipleInstances = true;
	
	if (LevelSong)
	{
		LevelSongSound = UGameplayStatics::CreateSound2D(this, LevelSong);
		FQuartzQuantizationBoundary QuantizationBoundary;
		QuantizationBoundary.Quantization = EQuartzCommandQuantization::Bar;
		LevelSongSound->PlayQuantized(
			this,
			TempoClock,
			QuantizationBoundary,
			FOnQuartzCommandEventBP());
	}
	
	TempoClock->SetBeatsPerMinute(
		this, 
		FQuartzQuantizationBoundary(),
		FOnQuartzCommandEventBP(),
		TempoClock,
		LevelBeatsPerMinute*2);
	TempoClock->StartClock(this, TempoClock);
	FOnQuartzMetronomeEventBP MetronomeEvent;
	MetronomeEvent.BindUFunction(this, TEXT("CreateMetronomeSounds"));
	
	TempoClock->SubscribeToAllQuantizationEvents(this, MetronomeEvent, TempoClock);
}

void ATempoLevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATempoLevelScript::CreateMetronomeSounds(FName ClockName, EQuartzCommandQuantization QuatizationType, int NumBars,
	int Beat, float BeatFraction)
{
	if (!bMetronomeOn) return;
	
	FQuartzQuantizationBoundary QuantizationBoundary;
	QuantizationBoundary.Quantization = EQuartzCommandQuantization::ThirtySecondNote;
	switch (QuatizationType)
	{
	case EQuartzCommandQuantization::Bar:
		HighBeepSound->PlayQuantized(this, TempoClock, QuantizationBoundary, FOnQuartzCommandEventBP());
		return;
	case EQuartzCommandQuantization::Beat:
		LowBeepSound->Play(0);
		return;
	default:
		return;
	}
}
