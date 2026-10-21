// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TestHealthComponent.h"
#include "TP_TestCharacter.generated.h"

class ATagBall;

UCLASS()
class UE5_BASICS_API ATP_TestCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ATP_TestCharacter();
	
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* MovementCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveVector* OffsetCurve;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	UTestHealthComponent* HealthComponent;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
	USceneComponent* StartFire;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline AimTimeline;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AimAction;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void FireFromWeapon();	
	
	UFUNCTION(BlueprintCallable, Category="Ball")
	void TryStealBall();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ball")
	ATagBall* HeldBall = nullptr;

	friend class ATagBall;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AimIn();
	void AimOut();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(BlueprintPure, Category = "Ball")
	bool HasBall() const;

private:
	UFUNCTION()
	void HandleProgressArmLength(float Length);
	
	UFUNCTION()
	void HandleProgressCameraOffset(FVector Offset);

};
