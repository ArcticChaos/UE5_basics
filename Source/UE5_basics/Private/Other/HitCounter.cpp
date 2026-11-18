#include "Other/HitCounter.h"

#include "TagBall.h"
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
	HitBox->SetNotifyRigidBodyCollision(true);
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
	RegisterBallHit(Cast<ATagBall>(OtherActor));
}

void AHitCounter::RegisterBallHit(ATagBall* Ball)
{
	if (!IsValid(Ball) || !Ball->IsInFlight())
	{
		return;
	}

	++HitCount;

	if (ATP_TestCharacter* Thrower = Ball->GetLastThrower(); IsValid(Thrower))
	{
		Thrower->AddScore(1);
	}

	Ball->Explode();

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