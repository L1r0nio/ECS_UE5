// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/1.Entity/TestEntity2.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnRegisterEntityMessage.h"


ATestEntity2::ATestEntity2(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	entity = objectInitializer.CreateDefaultSubobject<UEntity>(this, "TestEntity2");
	UEntityTypeComponent* entityTypeComponent = entity->AddComponent<UEntityTypeComponent>();
	entityTypeComponent->EntityType = EEntityType::TestActor2;
}


void ATestEntity2::BeginPlay()
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


