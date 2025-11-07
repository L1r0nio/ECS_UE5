// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"



UEntity::UEntity()
{
	InitializeComponentArray();
	componentMask = 0;
	bIsActive = true;
}



void UEntity::SetActive(const bool bNewActive)
{
	bIsActive = bNewActive;
}

bool UEntity::IsActive() const
{
	return bIsActive;
}



AActor* UEntity::GetActor() const
{
	return boundActor.Get();
}

void UEntity::SetActor(AActor* actor)
{
	boundActor = actor;
}



bool UEntity::HasComponents(const ComponentMask mask) const
{
	return (componentMask & mask) == mask;
}

ComponentMask UEntity::GetComponentMask() const
{
	return componentMask;
}

void UEntity::InitializeComponentArray()
{
	components.Empty();
	components.SetNum(ECSConstants::MAX_COMPONENT_TYPES);

	for (UComponent*& component : components)
		component = nullptr;
}
