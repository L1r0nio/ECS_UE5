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

bool UTestSystem1::GetAdditionalConditions(UEntity* entity) const
{
	return entity->GetComponent<UEntityTypeComponent>()->EntityType == EEntityType::TestActor1;
}



void UTestSystem1::Update(UEntity* entity, const float deltaTime)
{
	const EEntityType entityType = entity->GetComponent<UEntityTypeComponent>()->EntityType;
	const FString entityName = entity->GetActor()->GetName();
	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("__UTestSystem1__Entity type:%s \nHave name: %s"),
			*UEnum::GetValueAsString(entityType), *entityName));
}
