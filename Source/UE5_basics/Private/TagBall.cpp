#include "TagBall.h"

#include "TP_TestCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ATagBall::ATagBall()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SetRootComponent(PickupSphere);

	PickupSphere->InitSphereRadius(75.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphere->SetGenerateOverlapEvents(true);

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(PickupSphere);
	BallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATagBall::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ATagBall::HandleBeginOverlap);

	// Gestisce anche un player già sopra la palla all'avvio.
	PickupSphere->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	PickupSphere->GetOverlappingActors(
		OverlappingActors, ATP_TestCharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (IsValid(Holder))
		{
			break;
		}

		if (GiveTo(Cast<ATP_TestCharacter>(Actor)))
		{
			break;
		}
	}
}

void ATagBall::HandleBeginOverlap(
	AActor* OverlappedActor, AActor* OtherActor)
{
	// Una palla già raccolta si può rubare solo sparando.
	if (!IsValid(Holder))
	{
		GiveTo(Cast<ATP_TestCharacter>(OtherActor));
	}
}

bool ATagBall::GiveTo(ATP_TestCharacter* NewHolder)
{
	if (!IsValid(NewHolder) || Holder == NewHolder ||
		IsValid(NewHolder->HeldBall))
	{
		return false;	
	}

	if (IsValid(Holder))
	{
		Holder->HeldBall = nullptr;
	}

	Holder = NewHolder;
	Holder->HeldBall = this;

	SetActorEnableCollision(false);

	AttachToComponent(
		Holder->GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	SetActorRelativeLocation(CarryOffset);
	SetActorRelativeRotation(FRotator::ZeroRotator);

	return true;
}