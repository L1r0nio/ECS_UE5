// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/Movement/MovementSystem.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"



#pragma region SYSTEM

void UMovementSystem::Initialize()
{
	Super::Initialize();
	mainPlayerEntity = nullptr;
	bNeedEntities = false;
	bRequiresMainThread = true;
	bSupportsParallelExecution = false;
}

void UMovementSystem::Shutdown()
{
	Super::Shutdown();
	
	if (mainPlayerEntity)
		mainPlayerEntity = nullptr;
}


ComponentMask UMovementSystem::GetNeedComponents() const
{
	return UEntityTypeComponent::StaticTypeID() | UMainPlayerMovementComponent::StaticTypeID();
}

bool UMovementSystem::GetAdditionalConditions(UEntity* entity) const
{
	return entity->GetComponent<UEntityTypeComponent>()->EntityType == EEntityType::MainPlayer;
}


void UMovementSystem::RegisterEntity(UEntity* entity)
{
	if (mainPlayerEntity != nullptr)
		return;
	
	if (ShouldProcessEntity(entity))
	{
		mainPlayerEntity = entity;
		SetEnable(true);
	}
}

void UMovementSystem::UnregisterEntity(UEntity* entity)
{
	if (mainPlayerEntity != nullptr && entity == mainPlayerEntity)
	{
		mainPlayerEntity = nullptr;
		SetEnable(false);
	}
}


void UMovementSystem::Update(const float deltaTime)
{
	if (mainPlayerEntity == nullptr)
		return;

	if (UMainPlayerMovementComponent* movementComponent =
		mainPlayerEntity->GetComponent<UMainPlayerMovementComponent>())
	{
		if (ACharacter* mainPlayer = mainPlayerEntity->GetCharacter())
		{
			Look(*movementComponent, *mainPlayer);
			Walk(*movementComponent, *mainPlayer);
		}
	}
}

#pragma endregion



#pragma region MOVEMENT

void UMovementSystem::Walk(const UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const
{
	if (!movementComponent.bIsEnableMovement || movementComponent.MovementStates.MoveType == EMovementState::Place)
		return;
	
	const FMovementState& states = movementComponent.MovementStates;
	SetSpeed(states.MoveType);
	bool bIsDiagonal = false;
	const FVector2D localDirection = GetLocalDirection(states, bIsDiagonal);
	
	if (localDirection.IsNearlyZero())
		return;

	const FVector forwardVector = mainPlayer.GetActorForwardVector();
	const FVector rightVector = mainPlayer.GetActorRightVector();
	FVector worldDirection = forwardVector * localDirection.X + rightVector * localDirection.Y;

	worldDirection.Normalize();

	const float scalar = bIsDiagonal ? MainPlayerConst::WALK_DIAGONAL_SCALAR : MainPlayerConst::WALK_LINE_SCALAR;

	mainPlayer.AddMovementInput(worldDirection, scalar);
}

void UMovementSystem::Look(UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const
{
	if (!movementComponent.bIsEnableLook)
		return;

	FVector2D& lookAxis = movementComponent.LookAxis;
	
	mainPlayer.AddControllerYawInput( lookAxis.X * MainPlayerConst::MOUSE_SENSITIVITY);
	mainPlayer.AddControllerPitchInput(lookAxis.Y * MainPlayerConst::MOUSE_SENSITIVITY);

	if(!MainPlayerConst::IS_DRIFT_LOOK)
		lookAxis.X = lookAxis.Y = 0.0f;
}

#pragma endregion



#pragma region ADDITIONAL

FVector2D UMovementSystem::GetLocalDirection(const FMovementState& movementState, bool& bIsDiagonal) const
{
	using enum EMovementState;
	using namespace MainPlayerConst;
	
	FVector2D result = FVector2D::ZeroVector;

	auto addDir = [&result](const EMovementState dir)
	{
		switch (dir)
		{
			default: break;
			case Forward: result.X += MOVE_X; break;
			case Backward: result.X += -MOVE_X; break;
			case Right: result.Y += MOVE_Y; break;
			case Left: result.Y += -MOVE_Y; break;
		}
	};

	addDir(movementState.MoveOneDirection);
	addDir(movementState.MoveTwoDirection);

	bIsDiagonal = movementState.MoveOneDirection != None &&
				  movementState.MoveTwoDirection != None &&
				  result.SizeSquared() > KINDA_SMALL_NUMBER;

	return result;
}

void UMovementSystem::SetSpeed(const EMovementState state) const
{
	using enum EMovementState;
	using namespace MainPlayerConst;
	
	if (const ACharacter* mainPlayer = mainPlayerEntity->GetCharacter())
	{
		float& maxWalkSpeed = mainPlayer->GetCharacterMovement()->MaxWalkSpeed;
	
		switch (state)
		{
			default: break;
			case Place:maxWalkSpeed = WALK_SPEED;break;
			case Walk: maxWalkSpeed = WALK_SPEED;break;
			case Crouching: maxWalkSpeed = CROUCH_SPEED;break;
			case UnCrouching: maxWalkSpeed =WALK_SPEED;break;
			case Crouch: maxWalkSpeed = CROUCH_SPEED;break;
		}
	}
}

#pragma endregion
