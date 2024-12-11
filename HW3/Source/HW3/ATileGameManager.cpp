// Fill out your copyright notice in the Description page of Project Settings.


#include "ATileGameManager.h"

// Sets default values
AATileGameManager::AATileGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATileGameManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AATileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

