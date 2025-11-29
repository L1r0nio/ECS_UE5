// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/Movement/MovementSystem.h"
#include "Components/CapsuleComponent.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"



#pragma region SYSTEM

UMovementSystem::UMovementSystem()
{
	UMovementSystem::Initialize();

	static ConstructorHelpers::FObjectFinder<UCurveFloat> crouchCurveAlphaObject
		(TEXT("/Game/_ASETS_/--Graph--/CF_CrouchAlpha.CF_CrouchAlpha"));

	if (crouchCurveAlphaObject.Succeeded())
	{
		crouchCurveAlpha = crouchCurveAlphaObject.Object;
		crouchCurveAllTime = crouchCurveAlpha->FloatCurve.GetLastKey().Time;
	}
	
}

UMovementSystem::~UMovementSystem()
{
	UMovementSystem::Shutdown();
}


void UMovementSystem::Initialize()
{
	mainPlayerEntity = nullptr;
	crouchCurveAlpha = nullptr;
	crouchCurrentTime = 0.0f;
	crouchCurveAllTime = 0.0f;
	bNeedEntities = false;
	bRequiresMainThread = true;
	bSupportsParallelExecution = false;

	Super::Initialize();
}

void UMovementSystem::Shutdown()
{
	if (mainPlayerEntity)
	{
		mainPlayerEntity = nullptr;
		crouchCurveAlpha = nullptr;
	}

	Super::Shutdown();
}


ComponentMask UMovementSystem::GetNeedComponents() const
{
	return UEntityTypeComponent::StaticTypeID() | UMainPlayerMovementComponent::StaticTypeID();
}

bool UMovementSystem::GetAdditionalConditions(UEntity* entity) const
{
	if (mainPlayerEntity == nullptr)
		return entity->GetComponent<UEntityTypeComponent>()->EntityType == EEntityType::MainPlayer;

	return false;
}




void UMovementSystem::RegisterEntity(UEntity* entity)
{
	if (mainPlayerEntity != nullptr || !entity)
		return;

	if (ShouldProcessEntity(entity))
	{
		mainPlayerEntity = entity;
		SetEnable(true);
	}
}

void UMovementSystem::UnregisterEntity(UEntity* entity)
{
	if (mainPlayerEntity && entity && entity == mainPlayerEntity)
	{
		mainPlayerEntity = nullptr;
		crouchCurveAlpha = nullptr;
		SetEnable(false);
	}
}


void UMovementSystem::Update(const float deltaTime)
{
	if (!IsSystemReady())
		return;

	if (UMainPlayerMovementComponent* movementComponent =
		mainPlayerEntity->GetComponent<UMainPlayerMovementComponent>())
	{
		if (ACharacter* mainPlayer = mainPlayerEntity->GetCharacter())
		{
			Look(*movementComponent, *mainPlayer);
			Walk(*movementComponent, *mainPlayer);
			Addition(*movementComponent, *mainPlayer, deltaTime);
		}

		else
		{
			MainPlayerConst::bIsMovemetSystemUpdate = false;
		}
	}

	else
	{
		MainPlayerConst::bIsMovemetSystemUpdate = false;
	}
}

#pragma endregion



#pragma region MOVEMENT

void UMovementSystem::Walk(const UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const
{
	MainPlayerConst::bIsMovemetSystemUpdate = true;
	
	if (movementComponent.MovementStates.MoveType == EMovementState::Place)
	{
		MainPlayerConst::bIsMovemetSystemUpdate = false;
		return;
	}
	
	const FMovementState& states = movementComponent.MovementStates;
	
	if (states.Addition == EMovementState::None)
		UpdateMovementSpeed(states.MoveType, mainPlayer);

	bool bIsDiagonal = false;
	const FVector2D localDirection = GetLocalDirection(states, bIsDiagonal);

	if (localDirection.IsNearlyZero())
	{
		MainPlayerConst::bIsMovemetSystemUpdate = false;
		return;
	}
	
	const FVector worldDirection = CalculateWorldDirection(localDirection, mainPlayer);

	const float scalar = bIsDiagonal ? MainPlayerConst::WALK_DIAGONAL_SCALAR : MainPlayerConst::WALK_LINE_SCALAR;

	mainPlayer.AddMovementInput(worldDirection, scalar);
}

void UMovementSystem::Look(UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const
{
	FVector2D& lookAxis = movementComponent.LookAxis;

	mainPlayer.AddControllerYawInput(lookAxis.X * MainPlayerConst::MOUSE_SENSITIVITY);
	mainPlayer.AddControllerPitchInput(lookAxis.Y * MainPlayerConst::MOUSE_SENSITIVITY);

	if (!MainPlayerConst::IS_DRIFT_LOOK)
		lookAxis.X = lookAxis.Y = 0.0f;
}

void UMovementSystem::Addition(UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer,
	const float deltaTime) const
{
	switch (EMovementState& addition = movementComponent.MovementStates.Addition; addition)
	{
		default: break;
		case EMovementState::Jump:		  Jump(mainPlayer); break;
		case EMovementState::Crouching:
		case EMovementState::UnCrouching: Crouch(movementComponent, addition, mainPlayer, deltaTime); break;
	}
}


void UMovementSystem::Jump(ACharacter& mainPlayer) const
{
	MainPlayerConst::bIsMovemetSystemUpdate = true;
	mainPlayer.Jump();
	MainPlayerConst::bIsMovemetSystemUpdate = false;
}

void UMovementSystem::Crouch(const UMainPlayerMovementComponent& movementComponent, EMovementState& addition,
	const ACharacter& mainPlayer, const float deltaTime) const
{
	if (UCapsuleComponent* capsuleComponent = mainPlayer.GetCapsuleComponent())
	{
		if (USpringArmComponent* springArmComponent = movementComponent.SpringArm)
		{
			const bool bIsCrouching = addition == EMovementState::Crouching;
			UpdateMovementSpeed(addition, mainPlayer);
			ProcessCrouchTransition(*capsuleComponent, *springArmComponent, addition, bIsCrouching, deltaTime);
		}
	}
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
			case Forward: result.X += MOVE_X;	break;
			case Backward: result.X += -MOVE_X; break;
			case Right: result.Y += MOVE_Y;		break;
			case Left: result.Y += -MOVE_Y;		break;
		}
	};

	addDir(movementState.MoveOneDirection);
	addDir(movementState.MoveTwoDirection);

	bIsDiagonal = movementState.MoveOneDirection != None &&
		movementState.MoveTwoDirection != None &&
		result.SizeSquared() > KINDA_SMALL_NUMBER;

	return result;
}

FVector UMovementSystem::CalculateWorldDirection(const FVector2D& localDirection, const ACharacter& mainPlayer) const
{
	FVector worldDirection = mainPlayer.GetActorForwardVector() * localDirection.X +
		mainPlayer.GetActorRightVector() * localDirection.Y;

	worldDirection.Normalize();

	return worldDirection;
}


void UMovementSystem::ProcessCrouchTransition(UCapsuleComponent& capsuleComponent,
	USpringArmComponent& springArmComponent, EMovementState& addition, const bool bIsCrouching,
	const float deltaTime) const
{
	const float targetHalfHeight = bIsCrouching ?
		MainPlayerConst::CROUCH_CAPSULE_HALF_HEIGHT : MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT;

	const float targetSpringArmLocationZ = bIsCrouching ?
		MainPlayerConst::CROUCH_SPRING_ARM_LOCATION_Z : MainPlayerConst::STANDARD_SPRING_ARM_LOCATION_Z;

	const bool bCurveInvalid = crouchCurveAllTime <= 0.0f || crouchCurveAlpha == nullptr;
	const bool bFinished = crouchCurrentTime >= crouchCurveAllTime;
	
	if (bCurveInvalid || bFinished)
	{
		crouchCurrentTime = 0.0f; 
		capsuleComponent.SetCapsuleHalfHeight(targetHalfHeight);
		springArmComponent.SetRelativeLocation(FVector(0.0f, 0.0f, targetSpringArmLocationZ));
		addition = bIsCrouching ? EMovementState::Crouch : EMovementState::None;
		return;
	}
	
	const float sourceHalfHeight = bIsCrouching ?
		MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT : MainPlayerConst::CROUCH_CAPSULE_HALF_HEIGHT;

	const float sourceSpringArmLocationZ = bIsCrouching ?
		MainPlayerConst::STANDARD_SPRING_ARM_LOCATION_Z : MainPlayerConst::CROUCH_SPRING_ARM_LOCATION_Z;
	
	crouchCurrentTime = FMath::Clamp(crouchCurrentTime + deltaTime, 0.0f, crouchCurveAllTime);
	
	capsuleComponent.SetCapsuleHalfHeight(
		CurveInterpolateFloatValue(sourceHalfHeight, targetHalfHeight));

	springArmComponent.SetRelativeLocation(FVector(0.0f, 0.0f,
		CurveInterpolateFloatValue(sourceSpringArmLocationZ, targetSpringArmLocationZ)));
}

float UMovementSystem::CurveInterpolateFloatValue(const float oldValue, const float newValue) const
{
	if (FMath::IsNearlyEqual(oldValue, newValue) || crouchCurveAlpha == nullptr)
		return newValue;

	return FMath::Lerp(oldValue, newValue, crouchCurveAlpha->GetFloatValue(crouchCurrentTime));
}


void UMovementSystem::UpdateMovementSpeed(const EMovementState state, const ACharacter& mainPlayer) const
{
	const auto it = std::ranges::find_if(speedTable,
	                                     [state](const auto& pair) { return pair.first == state; });

	if (it != speedTable.end())
		mainPlayer.GetCharacterMovement()->MaxWalkSpeed = it->second;
}

#pragma endregion



#pragma region CAN

bool UMovementSystem::IsSystemReady() const
{
	return mainPlayerEntity != nullptr;
}

#pragma endregion