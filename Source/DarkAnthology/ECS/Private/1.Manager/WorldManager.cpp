// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/1.Manager/WorldManager.h"
#include "DarkAnthology/Game/Public/4.Message/EntityLife/RegisterUnregisterEntityMessage.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "DarkAnthology/ECS/Public/1.Manager/SystemManager.h"
#include "Public/3.System/Physics/Movement/MovementSystem.h"



#pragma region SETTINGS

void UWorldManager::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);
	CreateObject();
	Subscribe();
	
	UE_LOG(LogTemp, Log, TEXT("WorldManager Initialized"));
}

void UWorldManager::Deinitialize()
{
	Super::Deinitialize();
	UMessageBus::Shutdown();
	systemManager = nullptr;
	
	UE_LOG(LogTemp, Log, TEXT("WorldManager Deinitialized"));
}

void UWorldManager::CreateObject()
{
	UMessageBus::Initialize();
	
	systemManager = NewObject<USystemManager>(this, TEXT("USystemManager"));
	systemManager->Initialize();
	
	if (systemManager)
		RegisterSystem();
}

void UWorldManager::Subscribe()
{
	if (UMessageBus* bus = UMessageBus::Get())
	{
		bus->Subscribe<URegisterUnregisterEntityMessage>(this,
		[this](const URegisterUnregisterEntityMessage* message)
		{
			if (!message->UnRegisterEntity)
				RegisterEntity(message);

			else
				UnRegisterEntity(message);
		});
	}
}

#pragma endregion



#pragma region SYSTEM

void UWorldManager::RegisterSystem(USystem* system) const
{
	if (system)
		systemManager->RegisterSystem(system);
	
	
	systemManager->RegisterSystem<UMovementSystem>();
}

void UWorldManager::RegisterEntity(const URegisterUnregisterEntityMessage* message)
{
	if (message && message->Entity)
	{
		entitiesToAdd.Push(message->Entity);

		UE_LOG(LogTemp, Warning, TEXT("Entity %s registered"),
		       *message->Entity->GetActor()->GetName());
	}
}

void UWorldManager::UnRegisterEntity(const URegisterUnregisterEntityMessage* message)
{
	if (message && message->Entity)
		entitiesToRemove.Enqueue(message->Entity);

	UE_LOG(LogTemp, Log, TEXT("Entity %s UnRegistered"), *message->Entity->GetActor()->GetName());
}

void UWorldManager::UpdateWorld(const float deltaTime)
{
	if (!systemManager || systemManager->GetRegisteredSystems().IsEmpty())
		return;

	if (!entitiesToAdd.IsEmpty())
	{
		while (UEntity* entity = entitiesToAdd.Pop())
			systemManager->RegisterEntity(entity);
	}
	
	if (systemManager)
		systemManager->UpdateSystem(deltaTime);

	if (!entitiesToRemove.IsEmpty())
	{
		UEntity* entity = nullptr;

		while (entitiesToRemove.Dequeue(entity))
			systemManager->UnregisterEntity(entity);
	}
}

#pragma endregion