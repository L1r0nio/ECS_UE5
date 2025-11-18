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
	if (mainPlayerEntity != nullptr ||
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
	if (mainPlayerEntity != nullptr)
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
}

void AMainPlayerController::SetKeyForAction() const
{
	mainMappingContext->MapKey(moveAction, EKeys::W);
	mainMappingContext->MapKey(moveAction, EKeys::S);
	mainMappingContext->MapKey(moveAction, EKeys::A);
	mainMappingContext->MapKey(moveAction, EKeys::D);
}

void AMainPlayerController::SetModifierForKey() const
{
	constexpr uint8 moveActionWIndex = 0; 
	constexpr uint8 moveActionSIndex = 1;
	constexpr uint8 moveActionAIndex = 2;

	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionWIndex);
	AddModifierForAction<UInputModifierSwizzleAxis>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionSIndex);
	AddModifierForAction<UInputModifierNegate>(moveActionAIndex);
}

void AMainPlayerController::BindAction()
{
	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &AMainPlayerController::Move);
	}
}

#pragma endregion



#pragma region INPUT

void AMainPlayerController::Move(const FInputActionValue& value)
{
	if (mainPlayerEntity && movementComponent)
	{
		movementComponent->MovementStates.MoveState = value.Get<FVector2D>().IsZero() ? EMovementState::Place : EMovementState::Move;
	}
}

#pragma endregion



#pragma region ADDITIONAL

#pragma endregion