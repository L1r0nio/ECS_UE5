// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/Movement/CameraWalkTiltSystem.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/CameraWalkTiltPointComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"



#pragma region SYSTEM

UCameraWalkTiltSystem::UCameraWalkTiltSystem()
{
	UCameraWalkTiltSystem::Initialize();
}

UCameraWalkTiltSystem::~UCameraWalkTiltSystem()
{
	UCameraWalkTiltSystem::Shutdown();
}

void UCameraWalkTiltSystem::Initialize()
{
	cameraWalkTiltPoint = nullptr;
	mainPlayerMovementComponent = nullptr;
	bNeedEntities = false;
	bRequiresMainThread = true;
	bSupportsParallelExecution = false;

	Super::Initialize();
}

void UCameraWalkTiltSystem::Shutdown()
{
	if (cameraWalkTiltPoint && mainPlayerMovementComponent)
	{
		cameraWalkTiltPoint = nullptr;
		mainPlayerMovementComponent = nullptr;
	}

	Super::Shutdown();
}


ComponentMask UCameraWalkTiltSystem::GetNeedComponents() const
{
	return UCameraWalkTiltPointComponent::StaticTypeID() |
		   UEntityTypeComponent::StaticTypeID() |
		   UMainPlayerMovementComponent::StaticTypeID();
}

bool UCameraWalkTiltSystem::GetAdditionalConditions(UEntity* entity) const
{
	if (cameraWalkTiltPoint == nullptr && mainPlayerMovementComponent == nullptr)
		return entity->GetComponent<UEntityTypeComponent>()->EntityType == EEntityType::MainPlayer;
	
	return false;
}

void UCameraWalkTiltSystem::RegisterEntity(UEntity* entity)
{
	if (cameraWalkTiltPoint != nullptr && mainPlayerMovementComponent != nullptr)
		return;

	if (ShouldProcessEntity(entity))
	{
		cameraWalkTiltPoint = entity->GetComponent<UCameraWalkTiltPointComponent>()->CameraWalkTiltPoint;
		mainPlayerMovementComponent = entity->GetComponent<UMainPlayerMovementComponent>();
		SetEnable(true);
	}
}

void UCameraWalkTiltSystem::UnregisterEntity(UEntity* entity)
{
	if (cameraWalkTiltPoint && mainPlayerMovementComponent && entity &&
		entity->GetComponent<UCameraWalkTiltPointComponent>()->CameraWalkTiltPoint == cameraWalkTiltPoint)
	{
		cameraWalkTiltPoint = nullptr;
		mainPlayerMovementComponent = nullptr;
		SetEnable(false);
	}
}


void UCameraWalkTiltSystem::Update(const float deltaTime)
{
	if (!IsSystemReady())
		return;
	
	CameraTilt(deltaTime);
}

#pragma endregion



#pragma region ADDITIONAL

void UCameraWalkTiltSystem::CameraTilt(const float deltaTime) const
{
	const float targetAngle = GetTargetTiltAngle(mainPlayerMovementComponent->MovementStates);
	
	if (targetAngle == 0.0f)
	{
		SetDefaultRotation(deltaTime);
		return;
	}
	
	SetRotation(targetAngle, deltaTime);
}


void UCameraWalkTiltSystem::SetDefaultRotation(const float deltaTime) const
{
	if (cameraWalkTiltPoint->GetRelativeRotation().Roll == 0.0f)
		return;
	
	if (FMath::IsNearlyZero(cameraWalkTiltPoint->GetRelativeRotation().Roll, 0.01f))
	{
		cameraWalkTiltPoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		MainPlayerConst::IsCameraWalkTiltSystemUpdate = false;
		return;
	}

	SetRotation(0.0f, deltaTime);
}

float UCameraWalkTiltSystem::GetTargetTiltAngle(const FMovementState& state) const
{
	if (state.MoveState == EMovementState::Place || state.MoveTwoDirection == EMovementState::None)
		return 0.0f;
	
	switch (state.MoveTwoDirection)
	{
		default: return 0.0f;
		case EMovementState::Right: return MainPlayerConst::CAMERA_WALK_TILT_DEGREE;
		case EMovementState::Left:	return -MainPlayerConst::CAMERA_WALK_TILT_DEGREE;
	}
}

void UCameraWalkTiltSystem::SetRotation(const float newRotationX, const float deltaTime) const
{
	MainPlayerConst::IsCameraWalkTiltSystemUpdate = true;

	const float currentRotationX = cameraWalkTiltPoint->GetRelativeRotation().Roll;
	
	const float newValueX = FMath::FInterpTo(currentRotationX, newRotationX,
		deltaTime, MainPlayerConst::CAMERA_WALK_TILT_SPEED);
	
	cameraWalkTiltPoint->SetRelativeRotation(FRotator(0.0f, 0.0f, newValueX));
}

#pragma endregion



#pragma region CAN

bool UCameraWalkTiltSystem::IsSystemReady() const
{
	return cameraWalkTiltPoint &&
		   mainPlayerMovementComponent &&
		   MainPlayerConst::IS_CAMERA_WALK_TILT_ENABLE;
}

#pragma endregion