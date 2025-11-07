// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem2.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"



UTestSystem2::UTestSystem2()
{
	UTestSystem2::Initialize();
}

void UTestSystem2::Initialize()
{
	bNeedEntities = true;
	bSupportsParallelExecution = true;
	bRequiresMainThread = false;
	
	Super::Initialize();
}



ComponentMask UTestSystem2::GetNeedComponents() const
{
	return UEntityTypeComponent::StaticTypeID();
}

bool UTestSystem2::GetAdditionalConditions(UEntity* entity) const
{
	return entity->GetComponent<UEntityTypeComponent>()->EntityType == EEntityType::TestActor2;
}



void UTestSystem2::Update(UEntity* entity, const float deltaTime)
{
	const EEntityType entityType = entity->GetComponent<UEntityTypeComponent>()->EntityType;
	const FString entityName = entity->GetActor()->GetName();
	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red,
		FString::Printf(TEXT("__UTestSystem2__Entity type:%s \nHave name: %s"),
			*UEnum::GetValueAsString(entityType), *entityName));
}
