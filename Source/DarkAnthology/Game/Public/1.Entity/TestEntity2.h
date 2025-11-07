// L1 Game copyright. 2025. All rights reserved, probably :)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestEntity2.generated.h"

class UEntity;

UCLASS()
class DARKANTHOLOGY_API ATestEntity2 : public AActor
{
	GENERATED_BODY()

public:
	ATestEntity2(const FObjectInitializer& objectInitializer);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ECS|Entity")
	TObjectPtr<UEntity> entity;
};
