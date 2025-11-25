// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Physics/Movement/MovementSystem.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "GameFramework/MovementComponent.h"
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

	if (UMainPlayerMovementComponent* movementComponent = mainPlayerEntity->GetComponent<UMainPlayerMovementComponent>())
	{
		if (movementComponent->bIsEnableMovement)
		{
			const FMovementState& state = movementComponent->MovementStates;
	            
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
            				
            				
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("----------------------------"));
            				
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
			FString::Printf(TEXT(" Add: %s"), *UEnum::GetValueAsString(state.Addition)));
	            
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
			FString::Printf(TEXT(" Dir2: %s"), *UEnum::GetValueAsString(state.MoveTwoDirection)));
	            
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
			FString::Printf(TEXT(" Dir1: %s"), *UEnum::GetValueAsString(state.MoveOneDirection)));
	            
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
			FString::Printf(TEXT(" Type: %s"), *UEnum::GetValueAsString(state.MoveType)));
            				
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
			FString::Printf(TEXT(" State: %s"), *UEnum::GetValueAsString(state.MoveState)));
            			
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("--------MOVM STATE--------"));
		}
	}
}

#pragma endregion







/*if (UMainPlayerMovementComponent* movementComponent = mainPlayerEntity->GetComponent<UMainPlayerMovementComponent>())
{
const FMovementState& state = movementComponent->MovementStates;

GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
			
			
GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("----------------------------"));
			
GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
FString::Printf(TEXT(" Add: %s"), *UEnum::GetValueAsString(state.Addition)));

GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
FString::Printf(TEXT(" Dir2: %s"), *UEnum::GetValueAsString(state.MoveTwoDirection)));

GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
FString::Printf(TEXT(" Dir1: %s"), *UEnum::GetValueAsString(state.MoveOneDirection)));

GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
FString::Printf(TEXT(" Type: %s"), *UEnum::GetValueAsString(state.MoveType)));
			
GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
FString::Printf(TEXT(" State: %s"), *UEnum::GetValueAsString(state.MoveState)));
		
GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("--------MOVM STATE--------"));
}*/