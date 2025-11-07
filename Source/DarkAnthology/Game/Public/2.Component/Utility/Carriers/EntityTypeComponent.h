// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/Component.h"
#include "Enum/EEntityType.h"
#include "EntityTypeComponent.generated.h"


UCLASS()
class DARKANTHOLOGY_API UEntityTypeComponent : public UComponent
{
	GENERATED_BODY()
	DECLARE_TYPED_COMPONENT(UEntityTypeComponent)

public:
	UPROPERTY(VisibleAnywhere)
	EEntityType EntityType = EEntityType::None;
};