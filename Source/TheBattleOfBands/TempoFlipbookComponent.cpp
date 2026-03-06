// Fill out your copyright notice in the Description page of Project Settings.


#include "TempoFlipbookComponent.h"
#include "Musician.h"
#include "Quartz/QuartzSubsystem.h"

UTempoFlipbookComponent::UTempoFlipbookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTempoFlipbookComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnQuartzMetronomeEvent.BindDynamic(this, &UTempoFlipbookComponent::OnClockTick);
	
	UQuartzSubsystem* Quartz = GetWorld()->GetSubsystem<UQuartzSubsystem>();
	TempoClock = Quartz->GetHandleForClock(GetWorld(), TEXT("Tempo Clock"));
	TempoClock->SubscribeToQuantizationEvent(GetWorld(), EQuartzCommandQuantization::ThirtySecondNote, 
		OnQuartzMetronomeEvent, TempoClock);
	
	AActor* Owner = Cast<AActor>(GetOwner());
	if (Owner == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("NO OWNER"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetName());
	OwnerMusician = Cast<AMusician>(GetOwner());
	if (!OwnerMusician) UE_LOG(LogTemp, Error, TEXT("UH OH! FLIPBOOK WITH NO OWNER..."));
	
	StateFlipbooks = {
		{Idle, IdleFlipbook},
		{StartRun, StartRunFlipbook},
		{Running, RunningFlipbook},
		{Stopping, StoppingFlipbook},
		{Jumping, JumpRunningFlipbook},
		{Falling, FallRunningFlipbook},
		{Landing, LandRunningFlipbook},
	};
	
	SetFlipbook(IdleFlipbook.Flipbook);
	CurrentTempoFlipbook = IdleFlipbook;
}

void UTempoFlipbookComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsStateful) HandleAnimationState();
	if (!CurrentTempoFlipbook.Flipbook) return;
	if (CurrentTempoFlipbook.bIsTempoAnimation)
	{
		SetPlaybackPositionInFrames(FrameIndex, true);
		int32 DesiredFrameIndex = TickIndex % GetFlipbookLengthInFrames();
		if ((DesiredFrameIndex - FrameIndex > 1) && ++CatchUpTicks % CurrentTempoFlipbook.CatchUpLength)
		{
			FrameIndex++;
		}
		else
		{
			FrameIndex = DesiredFrameIndex;
		}
	}
}

void UTempoFlipbookComponent::SetAnimState(EAnimState NewAnimState)
{
	if (!CurrentTempoFlipbook.bIsTempoAnimation) GetWorld()->GetTimerManager().ClearTimer(FlipbookTimer);
	CurrentAnimState = NewAnimState;
	CurrentTempoFlipbook = *StateFlipbooks.Find(CurrentAnimState);
	SetFlipbook(CurrentTempoFlipbook.Flipbook);
	FrameIndex = 0;
	SetPlaybackPositionInFrames(FrameIndex, true);
	if (!CurrentTempoFlipbook.bIsTempoAnimation)
	{
		GetWorld()->GetTimerManager().SetTimer(FlipbookTimer, this,
			&UTempoFlipbookComponent::AdvancePlayRateFlipbook, 1.0/GetFlipbookFramerate(), true);
	}
}

void UTempoFlipbookComponent::OnClockTick(FName ClockName, EQuartzCommandQuantization QuantizationType, int NumBars,
	int Beat, float BeatFraction)
{
	if (NumBars > PrevBar)
	{
		TickIndex = 0;
		PrevBar = NumBars;
	}
	else
	{
		TickIndex++;
	}
}

void UTempoFlipbookComponent::HandleAnimationState()
{
	const FVector ActorVelocity = GetOwner()->GetVelocity();
	const float MoveSpeed = FVector2d(ActorVelocity.X, ActorVelocity.Y).Length();
	switch (CurrentAnimState)
	{
	case Idle:
		if (MoveSpeed != 0.f)
		{
			SetAnimState(StartRun);
		}
		break;
	case StartRun:
		if (MoveSpeed == 0.f)
		{
			SetAnimState(Idle);
		}
		else if (FrameIndex == 0)
		{
			SetAnimState(Running);
		}
		break;
	case Running:
		if (MoveSpeed < StopTransitionSpeed)
		{
			SetAnimState(Stopping);
		}
		break;
	case Stopping:
		if (MoveSpeed == 0.f && FrameIndex == 0)
		{
			SetAnimState(Idle);
		} else if (MoveSpeed > StopTransitionSpeed)
		{
			SetAnimState(Running);
		} else if (FrameIndex == 15)
		{
			FrameIndex = 14;
		}
		break;
	case Jumping:
		if (!OwnerMusician->IsFalling())
		{
			SetAnimState(Landing);
		}
		else if (FrameIndex == GetFlipbookLengthInFrames())
		{
			FrameIndex -= 2;
		}
		break;
	case Falling:
		if (!OwnerMusician->IsFalling())
		{
			SetAnimState(Landing);
		}
		else if (FrameIndex == GetFlipbookLengthInFrames())
		{
			FrameIndex -= 2;
		}
		break;
	case Landing:
		if (FrameIndex == GetFlipbookLengthInFrames())
		{
			SetAnimState(Idle);
		}
	}
}

void UTempoFlipbookComponent::AdvancePlayRateFlipbook()
{
	FrameIndex++;
	SetPlaybackPositionInFrames(FrameIndex, true);
}
