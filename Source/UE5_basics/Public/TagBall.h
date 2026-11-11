#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TagBall.generated.h"

class ATP_TestCharacter;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class UE5_BASICS_API ATagBall : public AActor
{
	GENERATED_BODY()

public:
	ATagBall();

	// Usata sia per la raccolta sia per il furto.
	bool GiveTo(ATP_TestCharacter* NewHolder);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball")
	USphereComponent* PickupSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball")
	UStaticMeshComponent* BallMesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ball")
	ATP_TestCharacter* Holder = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	FVector CarryOffset = FVector(0.0f, 0.0f, 130.0f);

private:
	UFUNCTION()
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};