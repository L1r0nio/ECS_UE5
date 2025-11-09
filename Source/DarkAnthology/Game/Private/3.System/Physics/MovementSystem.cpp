// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/MovementSystem.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"


#pragma region SYSTEM

UMovementSystem::UMovementSystem()
{
	UMovementSystem::Initialize();
}

void UMovementSystem::Initialize()
{
	Super::Initialize();
}

ComponentMask UMovementSystem::GetNeedComponents() const
{
	return UMainPlayerMovementComponent::StaticTypeID();
}

bool UMovementSystem::GetAdditionalConditions(UEntity* entity) const
{
	return true;
}

void UMovementSystem::Update(UEntity* entity, const float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "Movement");
	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("%f"), entity->GetComponent<UMainPlayerMovementComponent>()->LookAxis.X));
	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("%f"), entity->GetComponent<UMainPlayerMovementComponent>()->LookAxis.Y));
}

#pragma endregion
