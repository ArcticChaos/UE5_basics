#include "Other/HitCounter.h"

#include "TagBall.h"
#include "SoccerSubsystem.h"
#include "TP_TestCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

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

	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
	GoalMesh->SetupAttachment(HitBox);
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

void AHitCounter::SetGoalOwner(ATP_TestCharacter* InOwner, const FLinearColor& TeamColor)
{
	GoalOwner = InOwner;

	if (IsValid(GoalMesh))
	{
		const int32 NumMaterials = GoalMesh->GetNumMaterials();
		for (int32 MatIdx = 0; MatIdx < NumMaterials; ++MatIdx)
		{
			UMaterialInstanceDynamic* DynMaterial = GoalMesh->CreateAndSetMaterialInstanceDynamic(MatIdx);
			if (IsValid(DynMaterial))
			{
				DynMaterial->SetVectorParameterValue(TEXT("Color"), TeamColor);
				DynMaterial->SetVectorParameterValue(TEXT("BaseColor"), TeamColor);
				DynMaterial->SetVectorParameterValue(TEXT("Tint"), TeamColor);
			}
		}
	}
}

void AHitCounter::RegisterBallHit(ATagBall* Ball)
{
	if (!IsValid(Ball) || !Ball->IsInFlight())
	{
		return;
	}

	++HitCount;

	if (UWorld* World = GetWorld())
	{
		if (USoccerSubsystem* SoccerSubsystem = World->GetSubsystem<USoccerSubsystem>())
		{
			SoccerSubsystem->RegisterGoalFromBall(Ball, this, 1);
		}
	}

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