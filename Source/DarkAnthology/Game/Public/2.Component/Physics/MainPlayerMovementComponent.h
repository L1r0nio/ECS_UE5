// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/Component.h"
#include "Struct/FMovementState.h"
#include "MainPlayerMovementComponent.generated.h"



class USpringArmComponent;



UCLASS()
class DARKANTHOLOGY_API UMainPlayerMovementComponent : public UComponent
{
	GENERATED_BODY()
	DECLARE_TYPED_COMPONENT(UMainPlayerMovementComponent)

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	FMovementState MovementStates = FMovementState();

	UPROPERTY(VisibleAnywhere)
	FVector2D LookAxis;

	UPROPERTY(VisibleAnywhere)
	bool bIsEnableMovement = true;

	UPROPERTY(VisibleAnywhere)
	bool bIsEnableLook = true;
};
