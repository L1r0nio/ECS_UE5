// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/Game/Public/1.Entity/TestEntity1.h"
#include "DarkAnthology/Game/Public/4.Message/EntityLife/RegisterUnRegisterEntityMessage.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"


ATestEntity1::ATestEntity1(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	entity = objectInitializer.CreateDefaultSubobject<UEntity>(this, "TestEntity1");

	UEntityTypeComponent* entityTypeComponent = entity->AddComponent<UEntityTypeComponent>();
	entityTypeComponent->EntityType = EEntityType::TestActor1;
}


void ATestEntity1::BeginPlay()
{
	Super::BeginPlay();

	if (entity)
	{
		entity->SetActor(this);
		
		if (UMessageBus* bus = UMessageBus::Get())
		{
			URegisterUnRegisterEntityMessage* message = NewObject<URegisterUnRegisterEntityMessage>(this);
			message->Setup(entity);
			bus->Publish(message);
		}
	}
}