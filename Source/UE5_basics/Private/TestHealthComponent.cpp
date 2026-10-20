// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHealthComponent.h"
// Sets default values for this component's properties
UTestHealthComponent::UTestHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTestHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	currentHealth = MaxHealth;
	
}


// Called every frame
void UTestHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// after TimeToStartRecovery if no damage has been taken, recover health
	if (eAutoRecovery && currentHealth < MaxHealth)
	{
		if (CurrentRecoveryTimer > 0.0f)
		{
			CurrentRecoveryTimer -= DeltaTime;
		}
		else
		{
			recoverHealth(RecoveryAmount * DeltaTime); 
		}
	}
}

void UTestHealthComponent::GetDamage(int amount)
{
	if (amount > 0)
	{
		currentHealth = FMath::Clamp(currentHealth - amount, 0, MaxHealth);

		// damage taken, reset recovery timer
		CurrentRecoveryTimer = TimeToStartRecovery;
		
		OnDamageTaken.Broadcast();
		
		if (currentHealth == 0)
		{
			OnHealthToZero.Broadcast();
		}
	}
}

void UTestHealthComponent::recoverHealth(float amount)
{
	currentHealth = FMath::Clamp(currentHealth + amount, 0.0f, MaxHealth);
}

