// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealtTestDelegate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_BASICS_API UTestHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestHealthComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int MaxHealth = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool eAutoRecovery = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float TimeToStartRecovery = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float RecoveryAmount = 10.0f;
	
	UPROPERTY(BlueprintAssignable)
	FHealtTestDelegate OnHealthtoZero;
	
	UPROPERTY(BlueprintAssignable)
	FHealtTestDelegate OnDamageTaken;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Health")	
	void GetDamage(int amount);
	
	UFUNCTION(BlueprintPure)
	int GetCurrentHealth();
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void recoverHealth(int amount);
	
private:
	int currentHealth;
		
};
