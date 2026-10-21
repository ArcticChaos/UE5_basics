// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "HitCounter.generated.h"

UCLASS()
class UE5_BASICS_API AHitCounter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitCounter();
	
	UFUNCTION()
	void HandleActorHit(AActor* SelfActor, AActor* OtherActor,
						FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* HitBox;
	
	int HitCount = 0;	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
