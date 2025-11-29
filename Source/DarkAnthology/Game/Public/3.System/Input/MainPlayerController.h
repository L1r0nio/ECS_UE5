// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "Public/2.Component/Physics/Enum/EMovementState.h"
#include "Public/3.System/Input/Trigger/DelayedHoldTrigger.h"
#include "MainPlayerController.generated.h"


struct FMovementState;
class UEntity;
class AMainPlayer;
class UMainPlayerMovementComponent;
class UInputAction;
class URegisterUnregisterEntityMessage;
class UInputModifier;
class UInputTrigger;
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
	TObjectPtr<UInputAction> lookAction;
	
	UPROPERTY(VisibleAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> moveAction;
	
	UPROPERTY(VisibleAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> jumpAction;

	UPROPERTY(VisibleAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> crouchAction;

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
	inline void SetModifierAndTriggerForKey() const;
	inline void BindAction();
	
	template <typename TModifier>
	void AddModifierForAction(const int32 index) const
	{
		static_assert(TIsDerivedFrom<TModifier, UInputModifier>::IsDerived,
			"TModifier must be derived from UInputModifier");
		
		mainMappingContext->GetMapping(index).Modifiers.Add(NewObject<TModifier>(mainMappingContext));
	}

	template <typename TTrigger>
	void AddTriggerForAction(const int32 index) const
	{
		static_assert(TIsDerivedFrom<TTrigger, UInputTrigger>::IsDerived,
			"TTrigger must be UInputTrigger or derived from it");
		
		mainMappingContext->GetMapping(index).Triggers.Add(NewObject<TTrigger>(mainMappingContext));
	}

	void Look(const FInputActionValue& value);
	void Move(const FInputActionValue& value);
	void StartJump(const FInputActionValue& value);
	void EndJump(const FInputActionValue& value);
	void Crouch(const FInputActionValue& value);

	void ApplyDirectionalInput(FMovementState& states, const float x, const float y) const;
	
	inline void SetMovementState(FMovementState& states,
		const EMovementState moveState = EMovementState::Place,
		const EMovementState moveType = EMovementState::None,
		const EMovementState dir1 = EMovementState::None,
		const EMovementState dir2 = EMovementState::None) const;

	bool IsOppositeButtonPressed(const float x, const float y) const;
	inline bool IsSystemReady() const;
	inline bool CanWalk() const;
	inline bool CanLook() const;
	inline bool CanJump() const;
	inline bool CanCrouch() const;
	inline bool CanStandUp() const;

	inline void ResetLook() const;
	inline void ResetMove() const;
};