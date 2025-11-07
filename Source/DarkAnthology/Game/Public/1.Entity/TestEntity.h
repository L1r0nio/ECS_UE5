// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestEntity.generated.h"


class UEntity;

UCLASS()
class DARKANTHOLOGY_API ATestEntity : public AActor
{
	GENERATED_BODY()

public:
	ATestEntity(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ECS|Entity")
	UEntity* entity;
};
