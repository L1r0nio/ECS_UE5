// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Public/2.Component/Physics/Enum/EMovementState.h"
#include "MainPlayerController.generated.h"



class FClimbResult;
class UInputAction;
class UInputMappingContext;
struct FClimbTraceHitsResults;
struct FClimbValidationParams;
class UMainPlayerMovementComponent;
struct FCollisionQueryParams;
struct FInputActionValue;
struct FMovementState;
struct FHitResult;



/**For only MainPlayer*/
UCLASS()
class DARKANTHOLOGY_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputMappingContext> mainMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputAction> lookAction;

private:
	void CreateObject();
	void SetMappingContext() const;
	void SetKeyForAction() const;
	void SetModifierForKey() const;
	void BindAction();

	void Look(const FInputActionValue& value);
	void Move(const FInputActionValue& value);

	void LineInput(FMovementState& states, const float movementVectorX, const float movementVectorY);
	void DiagonalInput(FMovementState& states, const float movementVectorX, const float movementVectorY);

	void SetWalkMovementState(
		FMovementState& states,
		const EMovementState moveState = EMovementState::Place,
		const EMovementState moveType = EMovementState::None,
		const EMovementState dir1 = EMovementState::None,
		const EMovementState dir2 = EMovementState::None);


	bool IsKeyPressed(const FKey& key) const;
	bool IsOppositeButtonPressed(const FVector2D& movementVector) const;
};
