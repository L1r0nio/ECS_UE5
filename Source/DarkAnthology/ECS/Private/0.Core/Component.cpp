// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/0.Core/Component.h"


UComponent::UComponent()
{
	SetFlags(RF_NoFlags);
}

ComponentTypeID UComponent::GetTypeID() const
{
	return 0;
}

ComponentMask UComponent::GetMask() const
{
	return 1ULL << GetTypeID();
}