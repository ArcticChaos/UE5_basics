#include "Other/HitCounter.h"

#include "TP_TestCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

AHitCounter::AHitCounter()
{
	PrimaryActorTick.bCanEverTick = false;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	SetRootComponent(HitBox);

	HitBox->SetBoxExtent(FVector(100.0f));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitBox->SetCollisionResponseToAllChannels(ECR_Block);
	HitBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AHitCounter::BeginPlay()
{
	Super::BeginPlay();

	OnActorHit.AddDynamic(this, &AHitCounter::HandleActorHit);
}

void AHitCounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHitCounter::HandleActorHit(
	AActor* SelfActor,
	AActor* OtherActor,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	const ATP_TestCharacter* Player = Cast<ATP_TestCharacter>(OtherActor);

	if (!IsValid(Player) || !Player->HasBall())
	{
		return;
	}

	++HitCount;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("HitCount: %d"), HitCount));
	}

	UE_LOG(LogTemp, Warning, TEXT("HitCount: %d"), HitCount);
}