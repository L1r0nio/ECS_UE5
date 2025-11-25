// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Public/2.Component/Physics/Enum/EMovementState.h"
#include "MainPlayerController.generated.h"


struct FMovementState;
class UEntity;
class AMainPlayer;
class UMainPlayerMovementComponent;
class UInputAction;
class URegisterUnregisterEntityMessage;
class UInputModifier;
struct FInputActionValue;



UCLASS()
class DARKANTHOLOGY_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();
	
protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> mainMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> lookAction;
 
private:
	UPROPERTY()
	TObjectPtr<UEntity> mainPlayerEntity;
	
	UPROPERTY()
	TObjectPtr<UMainPlayerMovementComponent> movementComponent;

	inline void Subscribe();
	void RegisterEntity(const URegisterUnregisterEntityMessage* message);
	void UnRegisterEntity(const URegisterUnregisterEntityMessage* message);
	
	inline void CreateObject();
	inline void SetMappingContext() const;
	inline void SetActionValue() const;
	inline void SetKeyForAction() const;
	inline void SetModifierForKey() const;
	inline void BindAction();
	
	template <typename TModifier>
	void AddModifierForAction(const int32 index) const
	{
		static_assert(std::is_base_of_v<UInputModifier, TModifier>, "T must be derived from UInputModifier");
		mainMappingContext->GetMapping(index).Modifiers.Add(NewObject<TModifier>(mainMappingContext));
	}

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);

	inline void ApplyDirectionalInput(FMovementState& states, const float x, const float y) const;
	
	inline void SetMovementState(FMovementState& states,
		const EMovementState moveState = EMovementState::Place,
		const EMovementState moveType = EMovementState::None,
		const EMovementState dir1 = EMovementState::None,
		const EMovementState dir2 = EMovementState::None) const;

	bool IsOppositeButtonPressed(const float x, const float y) const;
};