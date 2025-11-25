// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Input/MainPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnregisterEntityMessage.h"



#pragma region SETTINGS

AMainPlayerController::AMainPlayerController()
{
	CreateObject();
	Subscribe();
}

void AMainPlayerController::CreateObject()
{
	mainPlayerEntity = nullptr;
	movementComponent = nullptr;
	mainMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Main"));
	moveAction = NewObject<UInputAction>(this, TEXT("IMC_Move"));
	lookAction = NewObject<UInputAction>(this, TEXT("IMC_Look"));
}

void AMainPlayerController::Subscribe()
{
	if (UMessageBus* messageBus = UMessageBus::Get())
	{
		messageBus->Subscribe<URegisterUnregisterEntityMessage>(this,
		[this](const URegisterUnregisterEntityMessage* message)
		{
			if (message && !message->UnRegisterEntity)
			{
				RegisterEntity(message);
			}

			else
			{
				UnRegisterEntity(message);
			}
		});
	}
}

#pragma endregion



#pragma region SYSTEM

void AMainPlayerController::RegisterEntity(const URegisterUnregisterEntityMessage* message)
{
	if (mainPlayerEntity ||
		message->Entity->GetComponent<UEntityTypeComponent>()->EntityType != EEntityType::MainPlayer)
	{
		return;
	}

	
	mainPlayerEntity = message->Entity;

	if (mainPlayerEntity)
		movementComponent = mainPlayerEntity->GetComponent<UMainPlayerMovementComponent>();
}

void AMainPlayerController::UnRegisterEntity(const URegisterUnregisterEntityMessage* message)
{
	if (mainPlayerEntity && movementComponent)
	{
		mainPlayerEntity = nullptr;
		movementComponent = nullptr;
	}
}

#pragma endregion



#pragma region SETUP INPUT

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	SetMappingContext();
	SetActionValue();
	SetKeyForAction();
	SetModifierForKey();
	BindAction();
}

void AMainPlayerController::SetMappingContext() const
{
	if (UEnhancedInputLocalPlayerSubsystem* subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		subsystem->ClearAllMappings();
		subsystem->AddMappingContext(mainMappingContext, 0);

		if (subsystem->HasMappingContext(mainMappingContext))
			UE_LOG(LogTemp, Warning, TEXT("mainMappingContext registred"));
	}
}


void AMainPlayerController::SetActionValue() const
{
	moveAction->ValueType = EInputActionValueType::Axis2D;
	lookAction->ValueType = EInputActionValueType::Axis2D;
}

void AMainPlayerController::SetKeyForAction() const
{
	mainMappingContext->MapKey(moveAction, EKeys::W);
	mainMappingContext->MapKey(moveAction, EKeys::S);
	mainMappingContext->MapKey(moveAction, EKeys::A);
	mainMappingContext->MapKey(moveAction, EKeys::D);

	mainMappingContext->MapKey(lookAction, EKeys::Mouse2D);
}

void AMainPlayerController::SetModifierForKey() const
{
	constexpr uint8 moveActionWIndex = 0; 
	constexpr uint8 moveActionSIndex = 1;
	constexpr uint8 moveActionAIndex = 2;
	constexpr uint8 lookActionIndex = 4;

	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionWIndex);
	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionAIndex);

	mainMappingContext->GetMapping(lookActionIndex).Modifiers.Add(
	([](){
		UInputModifierNegate* modifier = NewObject<UInputModifierNegate>(); 
		modifier->bY = true;
		modifier->bX = false; 
		modifier->bZ = false; 
		return modifier;
	})());
}

void AMainPlayerController::BindAction()
{
	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &AMainPlayerController::Move);

		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);
	}
}

#pragma endregion



#pragma region INPUT

void AMainPlayerController::Move(const FInputActionValue& value)
{
	if (mainPlayerEntity == nullptr || movementComponent == nullptr ||  !movementComponent->bIsEnableMovement)
		return;
	
	FMovementState& states = movementComponent->MovementStates;
	const FVector2D movementVector = value.Get<FVector2D>();
		
	if (movementVector.IsZero() || IsOppositeButtonPressed(movementVector.X, movementVector.Y))
	{
		SetMovementState(states, EMovementState::Place);
		return;
	}

	ApplyDirectionalInput(states, movementVector.X, movementVector.Y);
}

void AMainPlayerController::Look(const FInputActionValue& value)
{
	if (mainPlayerEntity == nullptr || movementComponent == nullptr ||  !movementComponent->bIsEnableLook)
		return;

	movementComponent->LookAxis.X = value.Get<FVector2D>().X;
	movementComponent->LookAxis.Y = value.Get<FVector2D>().Y;
}

#pragma endregion



#pragma region ADDITIONAL

void AMainPlayerController::ApplyDirectionalInput(FMovementState& states, const float x, const float y) const
{
	using enum EMovementState;
	
	const EMovementState directionOne =
		(y > 0.0f) ? Forward :
		(y < 0.0f) ? Backward :
		None;

	const EMovementState directionTwo =
		(x > 0.0f) ? Right :
		(x < 0.0f) ? Left :
		None;

	SetMovementState(states, Move, Walk, directionOne, directionTwo);
}


void AMainPlayerController::SetMovementState(FMovementState& states, const EMovementState moveState,
	const EMovementState moveType, const EMovementState dir1, const EMovementState dir2) const
{
	states.MoveState = moveState;
	states.MoveType = moveType;
	states.MoveOneDirection = dir1;
	states.MoveTwoDirection = dir2;
}

bool AMainPlayerController::IsOppositeButtonPressed(const float x, const float y) const
{
	const bool oppositeX =
		(x > 0.0f && IsInputKeyDown(EKeys::A)) ||
		(x < 0.0f && IsInputKeyDown(EKeys::D));

	const bool oppositeY =
		(y > 0.0f && IsInputKeyDown(EKeys::S)) ||
		(y < 0.0f && IsInputKeyDown(EKeys::W));

	return oppositeX || oppositeY;
}

#pragma endregion
