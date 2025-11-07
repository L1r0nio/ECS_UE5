// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestEntity1.generated.h"


class UEntity;

UCLASS()
class DARKANTHOLOGY_API ATestEntity1 : public AActor
{
	GENERATED_BODY()

public:
	ATestEntity1(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ECS|Entity")
	TObjectPtr<UEntity> entity;
};
