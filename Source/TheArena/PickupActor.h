// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PickupActor.generated.h"

UCLASS()
class THEARENA_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	FRotator RotationVector;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Pickup)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Pickup)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	UBoxComponent* BoxCollider;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);
};
