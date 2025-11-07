// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/Game/Public/1.Entity/TestEntity.h"
#include "DarkAnthology/Game/Public/4.Message/EntityLife/RegisterUnRegisterEntityMessage.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"


ATestEntity::ATestEntity(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	entity = objectInitializer.CreateDefaultSubobject<UEntity>(this, "TestEntity");

	UEntityTypeComponent* entityTypeComponent = entity->AddComponent<UEntityTypeComponent>();
	entityTypeComponent->EntityType = EEntityType::TestActor;
}


void ATestEntity::BeginPlay()
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