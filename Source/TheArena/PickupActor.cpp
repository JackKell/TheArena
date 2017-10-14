// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"
#include "TheArenaCharacter.h"


// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	this->RootComponent = SceneComponent;
	this->ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	this->ItemMesh->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	this->RotationVector = FRotator(0.0f, 180.0f, 0.0f);

	this->Speed = 1.0f;

	this->BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	this->BoxCollider->bGenerateOverlapEvents = true;
	this->BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnOverlapBegin);
	this->BoxCollider->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void APickupActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Check if the OtherActort is not this and if it is not NULL
	if (OtherActor != NULL && OtherActor != this && OtherComp != nullptr)
	{
		ATheArenaCharacter* player = Cast<ATheArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (OtherActor == player)
		{
			if (!player->IsStaminaFull())
			{
				player->ChangeStamina(10);
				Destroy();
			}
		}
	}
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->ItemMesh->AddLocalRotation(this->RotationVector * DeltaTime * Speed);
}

