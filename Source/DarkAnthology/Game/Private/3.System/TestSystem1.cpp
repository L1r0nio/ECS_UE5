// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem1.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"


UTestSystem1::UTestSystem1()
{
	UTestSystem1::Initialize();
}

void UTestSystem1::Initialize()
{
	bNeedEntities = true;
	bSupportsParallelExecution = false;
	bRequiresMainThread = true;

	Super::Initialize();
}

ComponentMask UTestSystem1::GetNeedComponents() const
{
	return UEntityTypeComponent::StaticTypeID();
}


void UTestSystem1::Update(const float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("Update"));
}

void UTestSystem1::Update(UEntity* entity, const float deltaTime)
{
	const EEntityType entityType = entity->GetComponent<UEntityTypeComponent>()->EntityType;
	const FString entityName = entity->GetName();
	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("Entity type:%s \nHave name: %s"),
			*UEnum::GetValueAsString(entityType), *entityName));
}