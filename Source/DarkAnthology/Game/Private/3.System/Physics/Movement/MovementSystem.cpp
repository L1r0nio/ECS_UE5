// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/Movement/MovementSystem.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"



#pragma region SYSTEM

void UMovementSystem::Initialize()
{
	Super::Initialize();
	mainPlayer = nullptr;
	bNeedEntities = false;
	bRequiresMainThread = true;
	bSupportsParallelExecution = false;
}

void UMovementSystem::Shutdown()
{
	Super::Shutdown();
	
	if (mainPlayer)
		mainPlayer = nullptr;
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
	if (mainPlayer != nullptr)
		return;
	
	if (ShouldProcessEntity(entity))
	{
		mainPlayer = entity;
		SetEnable(true);
	}
}

void UMovementSystem::UnregisterEntity(UEntity* entity)
{
	if (mainPlayer != nullptr && entity == mainPlayer)
	{
		mainPlayer = nullptr;
		SetEnable(false);
	}
}


void UMovementSystem::Update(const float deltaTime)
{
	if (mainPlayer == nullptr)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "MovementSystem");
}

#pragma endregion