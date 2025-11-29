// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/Component.h"
#include "UObject/Object.h"
#include "CameraWalkTiltPointComponent.generated.h"



class USceneComponent;



UCLASS()
class DARKANTHOLOGY_API UCameraWalkTiltPointComponent : public UComponent
{
	GENERATED_BODY()
	DECLARE_TYPED_COMPONENT(UCameraWalkTiltPointComponent)

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> CameraWalkTiltPoint = nullptr;
};
