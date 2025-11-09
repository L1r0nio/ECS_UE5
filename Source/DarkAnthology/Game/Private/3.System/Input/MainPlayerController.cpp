// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Input/MainPlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayer.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"


#pragma region SETTINGS

AMainPlayerController::AMainPlayerController()
{
	CreateObject();
}

void AMainPlayerController::CreateObject()
{
	mainMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Main"));
	lookAction = NewObject<UInputAction>(this, TEXT("IA_Look"));
	moveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	
	lookAction->ValueType = EInputActionValueType::Axis2D;
	moveAction->ValueType = EInputActionValueType::Axis2D;
}

#pragma endregion



#pragma region SETUP INPUT

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	SetMappingContext();
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
			UE_LOG(LogTemp, Display, TEXT("mainMappingContext setup"));
	}
}

void AMainPlayerController::SetKeyForAction() const
{
	mainMappingContext->MapKey(lookAction, EKeys::Mouse2D);

	mainMappingContext->MapKey(moveAction, EKeys::W);
	mainMappingContext->MapKey(moveAction, EKeys::S);
	mainMappingContext->MapKey(moveAction, EKeys::A);
	mainMappingContext->MapKey(moveAction, EKeys::D);
}

void AMainPlayerController::SetModifierForKey() const
{
	constexpr uint8 lookActionIndex = 0;
	constexpr uint8 moveActionWIndex = 1;
	constexpr uint8 moveActionSIndex = 2;
	constexpr uint8 moveActionAIndex = 3;

	mainMappingContext->GetMapping(lookActionIndex).Modifiers.Add(
		([]()
		{
			UInputModifierNegate* modifier = NewObject<UInputModifierNegate>();
			modifier->bY = true;
			modifier->bX = false;
			modifier->bZ = false;
			return modifier;
		})());

	mainMappingContext->GetMapping(moveActionWIndex).Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
	mainMappingContext->GetMapping(moveActionSIndex).Modifiers.Add(NewObject<UInputModifierSwizzleAxis>());
	mainMappingContext->GetMapping(moveActionSIndex).Modifiers.Add(NewObject<UInputModifierNegate>());
	mainMappingContext->GetMapping(moveActionAIndex).Modifiers.Add(NewObject<UInputModifierNegate>());
}

void AMainPlayerController::BindAction()
{
	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->
			BindAction(moveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
		
		enhancedInputComponent->
			BindAction(moveAction, ETriggerEvent::Completed, this, &AMainPlayerController::Move);

		enhancedInputComponent->
		BindAction(lookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);
	}
}

#pragma endregion



#pragma region INPUT

void AMainPlayerController::Look(const FInputActionValue& value)
{
	if (const AMainPlayer* player = Cast<AMainPlayer>(GetPawn()))
	{
		if (UMainPlayerMovementComponent* movementComponentComp =
			player->Entity->GetComponent<UMainPlayerMovementComponent>())
		{
			movementComponentComp->LookAxis.Y = value.Get<FVector2D>().Y;
			movementComponentComp->LookAxis.X = value.Get<FVector2D>().X;
		}
	}
}

void AMainPlayerController::Move(const FInputActionValue& value)
{
	if(const AMainPlayer* player = Cast<AMainPlayer>(GetPawn()))
	{
		if(UMainPlayerMovementComponent* movementComponent =
			player->Entity->GetComponent<UMainPlayerMovementComponent>())
		{
			if(!movementComponent->bIsEnableMovement)
				return;
			
			FMovementState& states = movementComponent -> MovementStates;
			
			const FVector2D movementVector = value.Get<FVector2D>();
			
			if (IsOppositeButtonPressed(movementVector))
			{
				SetWalkMovementState(states, EMovementState::Place);
				return;
			}
			
			SetWalkMovementState(states,value.Get<FVector2D>().IsNearlyZero() ?
				EMovementState::Place : EMovementState::Move);
			
			if(movementVector.X != 0.0f && movementVector.Y != 0.0f)
			{
				DiagonalInput(states, movementVector.X, movementVector.Y);
			}
			
			else if((movementVector.X != 0.0f && movementVector.Y == 0.0f) ||
					(movementVector.Y != 0.0f && movementVector.X == 0.0f))
			{
				LineInput(states, movementVector.X, movementVector.Y);
			}
		}
	}
}

#pragma endregion



#pragma region ADDITIONAL METOD

void AMainPlayerController::LineInput(FMovementState& states, const float movementVectorX,
	const float movementVectorY)
{
	if(movementVectorY > 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move, EMovementState::Walk, EMovementState::Forward);
				
	else if(movementVectorY < 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move, EMovementState::Walk,EMovementState::Backward);

	else if(movementVectorX > 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move, EMovementState::Walk,EMovementState::None, EMovementState::Right);

	else if(movementVectorX < 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move, EMovementState::Walk,EMovementState::None, EMovementState::Left);
}

void AMainPlayerController::DiagonalInput(FMovementState& states, const float movementVectorX,
	const float movementVectorY)
{
	if(movementVectorY > 0.0f && movementVectorX > 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move,EMovementState::Walk, EMovementState::Forward, EMovementState::Right);
	
	else if(movementVectorY > 0.0f && movementVectorX < 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move,EMovementState::Walk, EMovementState::Forward, EMovementState::Left);

	else if(movementVectorY < 0.0f && movementVectorX > 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move,EMovementState::Walk, EMovementState::Backward, EMovementState::Right);

	else if(movementVectorY < 0.0f && movementVectorX < 0.0f)
		SetWalkMovementState(states,
			EMovementState::Move,EMovementState::Walk,EMovementState::Backward, EMovementState::Left);
}


void AMainPlayerController::SetWalkMovementState(FMovementState& states, const EMovementState moveState,
const EMovementState moveType, const EMovementState dir1, const EMovementState dir2)
{
	states.MoveState = moveState;
	states.MoveType = moveType;
	states.MoveOneDirection = dir1;
	states.MoveTwoDirection = dir2;
}

bool AMainPlayerController::IsKeyPressed(const FKey& key) const
{
	if (GetLocalPlayer() && GetLocalPlayer()->GetPlayerController(GetWorld()))
		return GetLocalPlayer()->GetPlayerController(GetWorld())->IsInputKeyDown(key);

	return false;
}

bool AMainPlayerController::IsOppositeButtonPressed(const FVector2D& movementVector) const
{
	const bool isPressedTwoButtonsX = (movementVector.X > 0.0f && IsKeyPressed(EKeys::A)) || 
									  (movementVector.X < 0.0f && IsKeyPressed(EKeys::D));
	
	const bool isPressedTwoButtonsY = (movementVector.Y > 0.0f && IsKeyPressed(EKeys::S)) ||
									  (movementVector.Y < 0.0f && IsKeyPressed(EKeys::W));
	
	return isPressedTwoButtonsX || isPressedTwoButtonsY;
}

#pragma endregion
