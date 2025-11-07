// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/1.Manager/WorldManager.h"
#include "DarkAnthology/Game/Public/4.Message/EntityLife/RegisterUnRegisterEntityMessage.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "DarkAnthology/ECS/Public/1.Manager/SystemManager.h"
#include "Public/3.System/TestSystem1.h"
#include "Public/3.System/TestSystem2.h"
#include "Public/3.System/TestSystem3.h"
#include "Public/3.System/TestSystem4.h"
#include "Public/3.System/TestSystem5.h"


void UWorldManager::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	UMessageBus::Initialize();
	
	systemManager = NewObject<USystemManager>(this, TEXT("USystemManager"));
	systemManager->Initialize();

	if (UMessageBus* bus = UMessageBus::Get())
	{
		bus->Subscribe<URegisterUnRegisterEntityMessage>(this,
		[this](const URegisterUnRegisterEntityMessage* message)
		{
			if (!message->UnRegisterEntity)
				RegisterEntity(message);

			else
				UnRegisterEntity(message);
		});
	}

	if (systemManager)
		RegisterSystem();

	UE_LOG(LogTemp, Log, TEXT("WorldManager Initialized"));
}

void UWorldManager::Deinitialize()
{
	Super::Deinitialize();
	
	systemManager = nullptr;
	
	UMessageBus::Shutdown();
	
	UE_LOG(LogTemp, Log, TEXT("WorldManager Deinitialized"));
}



void UWorldManager::RegisterSystem(USystem* system) const
{
	if (system)
		systemManager->RegisterSystem(system);

	systemManager->RegisterSystem<UTestSystem1>();
	systemManager->RegisterSystem<UTestSystem2>();
	systemManager->RegisterSystem<UTestSystem3>();
	systemManager->RegisterSystem<UTestSystem4>();
	systemManager->RegisterSystem<UTestSystem5>();
}

void UWorldManager::RegisterEntity(const URegisterUnRegisterEntityMessage* message)
{
	if (message && message->Entity)
	{
		entitiesToAdd.Push(message->Entity);

		UE_LOG(LogTemp, Warning, TEXT("Entity %s registered"),
		       *message->Entity->GetActor()->GetName());
	}
}

void UWorldManager::UnRegisterEntity(const URegisterUnRegisterEntityMessage* message)
{
	if (message && message->Entity)
		entitiesToRemove.Enqueue(message->Entity);

	UE_LOG(LogTemp, Log, TEXT("Entity %s UnRegistered"), *message->Entity->GetActor()->GetName());
}



void UWorldManager::UpdateWorld(const float deltaTime)
{
	if (!systemManager || systemManager->RegisteredSystems.IsEmpty())
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