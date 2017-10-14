// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TheArenaCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

ATheArenaCharacter::ATheArenaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATheArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &ATheArenaCharacter::ToggleClimb);

	PlayerInputComponent->BindAction("FireLazer", IE_Pressed, this, &ATheArenaCharacter::FireLazer);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATheArenaCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATheArenaCharacter::StopSprint);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATheArenaCharacter::ToggleCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATheArenaCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATheArenaCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATheArenaCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATheArenaCharacter::LookUpAtRate);
}

void ATheArenaCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATheArenaCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATheArenaCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (bIsClimbing)
		{
			ClimbDirection = Value;
			FVector DestinationLocation = GetActorLocation();
			DestinationLocation.Z += 1.0f * Value;
			SetActorLocation(DestinationLocation, false);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Moving Up")));
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void ATheArenaCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		if (bIsClimbing)
		{
			FVector DestinationLocation = GetActorLocation();
			DestinationLocation.Y += 1.0f * Value;
			SetActorLocation(DestinationLocation, false);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Moving Up")));
		}
		else
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
	
			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void ATheArenaCharacter::ToggleCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ATheArenaCharacter::Sprint()
{
	// Don't allow the player to sprint if they drop below a certain stamina level
	if (CurrentStamina < MinimumSprintStamina) {
		return;
	}
	bIsSprinting = true;
	UnCrouch();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ATheArenaCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATheArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCanClimb();
	StaminaRegenerationCoolDownTimer += DeltaTime;
	
	// If the player is sprinting then drain their stamina
	if (bIsSprinting)
	{
		ChangeStamina(DeltaTime * SprintStaminaDrainRate * -1);
	}

	// If the player runs out of stamina
	if (CurrentStamina <= 0)
	{
		StopSprint();
	}

	if (StaminaRegenerationCoolDownTime <= StaminaRegenerationCoolDownTimer && !IsStaminaFull()) 
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::White, "Regenerating Stamina");
		ChangeStamina(DeltaTime * StaminaRegenerationRate);
	}
	ClimbDirection = 0;
}

void ATheArenaCharacter::ChangeStamina(float deltaStamina)
{
	PreviousStamina = CurrentStamina;
	CurrentStamina += deltaStamina; 
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, TotalStamina);
	// If Stamina was drained then reset the stamina regeneration timer to 0
	if (PreviousStamina > CurrentStamina) {
		StaminaRegenerationCoolDownTimer = 0.0f;
	}
}

bool ATheArenaCharacter::IsStaminaFull()
{
	return CurrentStamina >= TotalStamina;
}

void ATheArenaCharacter::FireLazer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Firing Lazer")));
	float Distance = 5000.0f;
	FHitResult* HitResult = new FHitResult();
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * Distance;
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, Start, End, ECC_Visibility, *TraceParams))
	{
		DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), true);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("You hit %s"), *HitResult->Actor->GetName()));
	}
}

void ATheArenaCharacter::UpdateCanClimb()
{
	float Distance = 70.0f;
	FHitResult* HitResult = new FHitResult();
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * Distance;
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	bool bHit = GetWorld()->LineTraceSingleByChannel(*HitResult, Start, End, ECC_Visibility, *TraceParams);

	if (bHit)
	{
		if (HitResult->Actor != this)
		{
			bCanClimb = true;
			DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), true);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("You hit %s"), *HitResult->Actor->GetName()));
		}
	}
	else 
	{
		bCanClimb = false;
		SetClimb(false);
	}
}

void ATheArenaCharacter::SetClimb(bool Climb)
{
	if (Climb) 
	{
		GetMovementComponent()->Velocity = FVector(0,0,0);
	}
	bIsClimbing = Climb;
	GetCharacterMovement()->GravityScale = Climb ? 0: 1;
}

void ATheArenaCharacter::ToggleClimb()
{
	// If currently climbing then let go of the wall
	if (bIsClimbing)
	{
		SetClimb(false);
	}
	else
	{
		// Else check if you can grab a wall
		if (bCanClimb)
		{
			SetClimb(true);
		}
	}
}