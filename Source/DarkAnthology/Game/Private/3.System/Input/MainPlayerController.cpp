// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Input/MainPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnregisterEntityMessage.h"
#include "Public/3.System/Input/Trigger/DelayedHoldTrigger.h"



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
	lookAction = NewObject<UInputAction>(this, TEXT("IMC_Look"));
	moveAction = NewObject<UInputAction>(this, TEXT("IMC_Move"));
	jumpAction = NewObject<UInputAction>(this, TEXT("IMC_Jump"));
	crouchAction = NewObject<UInputAction>(this, TEXT("IMC_Crouch"));
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
	SetModifierAndTriggerForKey();
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
	jumpAction->ValueType = EInputActionValueType::Boolean;
	crouchAction->ValueType = EInputActionValueType::Boolean;
}

void AMainPlayerController::SetKeyForAction() const
{
	mainMappingContext->MapKey(lookAction, EKeys::Mouse2D);

	mainMappingContext->MapKey(moveAction, EKeys::W);
	mainMappingContext->MapKey(moveAction, EKeys::S);
	mainMappingContext->MapKey(moveAction, EKeys::A);
	mainMappingContext->MapKey(moveAction, EKeys::D);

	mainMappingContext->MapKey(jumpAction, EKeys::SpaceBar);

	mainMappingContext->MapKey(crouchAction, EKeys::C);
}

void AMainPlayerController::SetModifierAndTriggerForKey() const
{
	constexpr uint8 lookActionIndex = 0;
	constexpr uint8 moveActionWIndex = 1;
	constexpr uint8 moveActionSIndex = 2;
	constexpr uint8 moveActionAIndex = 3;
	constexpr uint8 jumpActionIndex = 5;

	mainMappingContext->GetMapping(lookActionIndex).Modifiers.Add(
		([]()
		{
			UInputModifierNegate* modifier = NewObject<UInputModifierNegate>();
			modifier->bY = true;
			modifier->bX = false;
			modifier->bZ = false;
			return modifier;
		})());

	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionWIndex);
	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionAIndex);

	AddTriggerForAction<UDelayedHoldTrigger>(jumpActionIndex);
}

void AMainPlayerController::BindAction()
{
	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);

		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &AMainPlayerController::Move);

		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &AMainPlayerController::StartJump);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &AMainPlayerController::EndJump);

		enhancedInputComponent->BindAction(crouchAction, ETriggerEvent::Started, this, &AMainPlayerController::Crouch);
		enhancedInputComponent->BindAction(crouchAction, ETriggerEvent::Canceled, this, &AMainPlayerController::Crouch);
	}
}

#pragma endregion



#pragma region INPUT

void AMainPlayerController::Look(const FInputActionValue& value)
{
	if (!IsSystemReady() || !CanLook())
	{
		ResetLook();
		return;
	}

	movementComponent->LookAxis.X = value.Get<FVector2D>().X;
	movementComponent->LookAxis.Y = value.Get<FVector2D>().Y;
}

void AMainPlayerController::Move(const FInputActionValue& value)
{
	if (!IsSystemReady() || !CanWalk())
	{
		ResetMove();
		return;
	}
	

	FMovementState& states = movementComponent->MovementStates;
	const FVector2D movementVector = value.Get<FVector2D>();

	if (movementVector.IsZero() || IsOppositeButtonPressed(movementVector.X, movementVector.Y))
	{
		SetMovementState(states, EMovementState::Place);
		return;
	}

	ApplyDirectionalInput(states, movementVector.X, movementVector.Y);
}

void AMainPlayerController::StartJump(const FInputActionValue& value)
{
	if (!IsSystemReady() || !CanJump() || !value.Get<bool>())
		return;

	movementComponent->MovementStates.Addition = EMovementState::Jump;
}

void AMainPlayerController::EndJump(const FInputActionValue& value)
{
	if (!IsSystemReady())
		return;

	if (FMovementState& state = movementComponent->MovementStates; state.Addition == EMovementState::Jump)
		state.Addition = EMovementState::None;
}

void AMainPlayerController::Crouch(const FInputActionValue& value)
{
	if (!IsSystemReady() || !CanCrouch())
		return;
	
	switch (FMovementState& state = movementComponent->MovementStates; state.Addition)
	{
		default: break;
		case EMovementState::None: state.Addition = EMovementState::Crouching; break;
		case EMovementState::Crouch:
			{
				if (!CanStandUp())
					return;

				state.Addition = EMovementState::UnCrouching;
			} break;
	}
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

void AMainPlayerController::ResetLook() const
{
	movementComponent->LookAxis.X = 0.0f;
	movementComponent->LookAxis.Y = 0.0f;
}

void AMainPlayerController::ResetMove() const
{
	movementComponent->MovementStates.SetDefaultMovementState();
}

#pragma endregion



#pragma region CAN

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

bool AMainPlayerController::IsSystemReady() const
{
	return mainPlayerEntity && movementComponent;
}

bool AMainPlayerController::CanLook() const
{
	return movementComponent->bIsEnableLook;
}

bool AMainPlayerController::CanWalk() const
{
	return movementComponent->bIsEnableMovement;
}

bool AMainPlayerController::CanJump() const
{
	return movementComponent->bIsEnableMovement &&
		movementComponent->MovementStates.MoveOneDirection != EMovementState::Backward &&
		movementComponent->MovementStates.Addition == EMovementState::None;
}

bool AMainPlayerController::CanCrouch() const
{
	return movementComponent->bIsEnableMovement &&
			(movementComponent->MovementStates.Addition == EMovementState::None ||
			movementComponent->MovementStates.Addition == EMovementState::Crouch);
}

bool AMainPlayerController::CanStandUp() const
{
	const ACharacter* mainPlayer = mainPlayerEntity->GetCharacter();

	if (mainPlayer == nullptr)
		return false;
	
	const FVector boxSize = FVector(
		MainPlayerConst::STANDARD_CAPSULE_RADIUS,
		MainPlayerConst::STANDARD_CAPSULE_RADIUS,
		MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT - MainPlayerConst::CROUCH_CAPSULE_HALF_HEIGHT);
	
	const FVector traceStart = mainPlayer->GetActorLocation() +
		FVector(0.0f, 0.0f,MainPlayerConst::CROUCH_CAPSULE_HALF_HEIGHT * 2.0f);
	
	const FVector traceEnd = traceStart + FVector(0.0f, 0.0f,
		MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT - MainPlayerConst::CROUCH_CAPSULE_HALF_HEIGHT * 2.0f);

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(mainPlayer);
	collisionParams.bTraceComplex = false;

	const bool bIsHit = GetWorld()->SweepTestByChannel(
		traceStart,
		traceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(boxSize),
		collisionParams);

	if (MainPlayerConst::IS_SHOW_UNCROUCH_BOX_COLLISION)
		DrawDebugBox(
			mainPlayer->GetWorld(), traceEnd, boxSize, FColor::Red, false, 2.0f);
	
	return !bIsHit;
}

#pragma endregion