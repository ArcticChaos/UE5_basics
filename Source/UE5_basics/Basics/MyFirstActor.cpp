// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFirstActor.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMyFirstActor::AMyFirstActor()
{
 	PrimaryActorTick.bCanEverTick = true; // can tick?
	PrimaryActorTick.TickInterval = 0.2f; // what is the interval tick?

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	BoxComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyFirstActor::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AMyFirstActor::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AMyFirstActor::OnEndOverlap);
	
	// check overlapping collision on start and debug logs
	BoxComponent->UpdateOverlaps();
	BoxComponent->GetOverlappingActors(ActorsInside);
	UE_LOG(LogTemp, Warning, TEXT("Actors inside: %d"), ActorsInside.Num());
	
    // add ondestroyed event when actor is inside
	for (AActor* Actor : ActorsInside)
	{
		if (IsValid(Actor))
		{
			// Invece del normale AddDynamic, usiamo AddUniqueDynamic (spiegato sotto)
			Actor->OnDestroyed.AddUniqueDynamic(this, &AMyFirstActor::OnActorInsideDestroyed);
		}
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		EnableInput(PC);
		// Bind del tasto 'P' (P per Print). Equivalente di Input.GetKeyDown(KeyCode.P)
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::P, IE_Pressed, this, &AMyFirstActor::PrintActorsInside);
		}
	}
}

void AMyFirstActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(!IsValid(OtherActor))
	{
		return;
	}

	UClass* OtherActorClass = OtherActor->GetClass();
	for(auto& SensedClass : SensedCharacterClasses)
	{
		if (SensedClass && (OtherActorClass == SensedClass || OtherActorClass->IsChildOf(SensedClass)))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, "IN: "+OtherActor->GetName());
			OtherActor->OnDestroyed.AddUniqueDynamic(this, &AMyFirstActor::OnActorInsideDestroyed);
			ActorsInside.AddUnique(OtherActor);
			break;
		}
	}

}

void AMyFirstActor::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(!IsValid(OtherActor))
	{
		return;
	}

	UClass* OtherActorClass = OtherActor->GetClass();
	for(auto& SensedClass : SensedCharacterClasses)
	{
		if (SensedClass && (OtherActorClass == SensedClass || OtherActorClass->IsChildOf(SensedClass)))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "OUT: "+OtherActor->GetName());
			OtherActor->OnDestroyed.RemoveDynamic(this, &AMyFirstActor::OnActorInsideDestroyed);
			ActorsInside.Remove(OtherActor);
			break;
		}
	}
}

void AMyFirstActor::OnActorInsideDestroyed(AActor* DestroyedActor)
{
	ActorsInside.Remove(DestroyedActor);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Un attore e' stato distrutto mentre era dentro!");
}

void AMyFirstActor::BindTest()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "Player has shoot!");
}

void AMyFirstActor::PrintActorsInside()
{
	for (auto actor : ActorsInside)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, actor->GetName());
	}
}

// Called every frame
void AMyFirstActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
