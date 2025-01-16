// Fill out your copyright notice in the Description page of Project Settings.


#include "Eddie/Zoe.h"

// Sets default values
AZoe::AZoe()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AZoe::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZoe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AZoe::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

