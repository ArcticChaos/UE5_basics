// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHealthComponent.h"

#include "MathUtil.h"

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
	if (TimeToStartRecovery > 0.0f && eAutoRecovery)
	{
		TimeToStartRecovery -= DeltaTime;
		if (TimeToStartRecovery <= 0.0f)
		{
			recoverHealth(RecoveryAmount);
		}
	}
}

void UTestHealthComponent::GetDamage(int amount)
{
	if (amount < 0)
	{
		const auto actualDamage = FMathf::Abs(amount);
		currentHealth = FMath::Clamp(currentHealth - actualDamage, 0, MaxHealth);
		
		OnDamageTaken.Broadcast();
		
		if (currentHealth == 0)
		{
			OnHealthtoZero.Broadcast();
		}
	}
}

int UTestHealthComponent::GetCurrentHealth()
{
	return currentHealth;
}

void UTestHealthComponent::recoverHealth(int amount)
{
	currentHealth = FMath::Clamp(currentHealth + amount, 0, MaxHealth);
}

