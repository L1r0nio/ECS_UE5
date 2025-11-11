// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/1.Entity/Character/MainPlayer/MainPlayer.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnregisterEntityMessage.h"


#pragma region SETTINGS

AMainPlayer::AMainPlayer(const FObjectInitializer& objectInitializer)
{
	CreateObject(objectInitializer);
	SetSettings();
}

void AMainPlayer::CreateObject(const FObjectInitializer& objectInitializer)
{
	entity = objectInitializer.CreateDefaultSubobject<UEntity>(this, TEXT("MainPlayerEntity"));
}

void AMainPlayer::SetComponent()
{
	if (!entity)
		return;
	
	entity->SetActor(this);

	UEntityTypeComponent* entityTypeComponent = entity->AddComponent<UEntityTypeComponent>();
	entityTypeComponent->EntityType = EEntityType::MainPlayer;

	entity->AddComponent<UMainPlayerMovementComponent>();
}

void AMainPlayer::SetSettings()
{
	PrimaryActorTick.bCanEverTick = true;
}

#pragma endregion



#pragma region SYSTEM

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetComponent();
	SendMessage();
}

void AMainPlayer::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
	SendMessage(true);
	entity = nullptr;
}

#pragma endregion



#pragma region ADDITIONAL METOD

void AMainPlayer::SendMessage(const bool bIsUnregister)
{
	if (!entity)
		return;
	
	if (UMessageBus* bus = UMessageBus::Get())
	{
		URegisterUnregisterEntityMessage* message = NewObject<URegisterUnregisterEntityMessage>(this);
		message->Setup(entity, bIsUnregister);
		bus->Publish(message);
	}
}

UEntity* AMainPlayer::GetEntity() const
{
	return entity;
}

#pragma endregion