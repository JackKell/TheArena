// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "TheArenaCharacter.generated.h"

UCLASS(config=Game)
class ATheArenaCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ATheArenaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=TheArena)
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TheArena)
	bool bIsSneaking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TheArena)
	bool bIsClimbing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TheArena)
	bool bCanClimb = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TheArena)
	float ClimbDirection = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheArena)
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheArena)
	float WalkSpeed = 400.0f;

	// Stamina units gained per second while not using stamina
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheArena)
	float StaminaRegenerationRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TheArena)
	float StaminaRegenerationCoolDownTime = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TheArena)
	float StaminaRegenerationCoolDownTimer = 0.0f;

	// Stamina units lost per second while sprinting
	UPROPERTY(EditAnywhere, Category = TheArena)
	float SprintStaminaDrainRate = 5.0f;

	// Total minimum Stamina needed to sprint
	UPROPERTY(EditAnywhere, Category = TheArena)
	float MinimumSprintStamina = 10.0f;

	// Total amount of stamina in stamina pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TheArena)
	float TotalStamina = 100.0f;

	// Amount of stamina remaining
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TheArena)
	float CurrentStamina = TotalStamina;

	// Amount of stamina remaing last tick
	UPROPERTY(VisibleAnywhere, Category = TheArena)
	float PreviousStamina = CurrentStamina;

	UFUNCTION()
	bool IsStaminaFull();

protected:
	UFUNCTION()
	void MoveRight(float Value);
	
	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void ToggleCrouch();

	UFUNCTION()
	void Sprint();

	UFUNCTION()
	void StopSprint();

	UFUNCTION()
	void TurnAtRate(float Rate);

	UFUNCTION()
	void LookUpAtRate(float Rate);

	UFUNCTION()
	void Tick(float DeltaTime) override;

	UFUNCTION()
	void FireLazer();

	UFUNCTION()
	void UpdateCanClimb();

	UFUNCTION()
	void ToggleClimb();

	UFUNCTION()
	void SetClimb(bool Climb);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void ChangeStamina(float deltaStamina);
};

