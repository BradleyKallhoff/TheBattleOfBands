// Fill out your copyright notice in the Description page of Project Settings.


#include "Musician.h"

#include "TempoFlipbookComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMusician::AMusician()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root Component
	UCapsuleComponent* CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Root Component");
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCapsuleRadius(80.0f);
	CapsuleComponent->SetCapsuleHalfHeight(180.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	
	// Flipbook Component
	FlipbookComponent = CreateDefaultSubobject<UTempoFlipbookComponent>(TEXT("Tempo Flipbook"));
	FlipbookComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AMusician::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMusician::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMusician::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AMusician::IsFalling()
{
	return false;
}