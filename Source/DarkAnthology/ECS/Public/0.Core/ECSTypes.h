// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/2.Util/Profiler.h"


using EntityID = uint32;
using ComponentMask = uint64;
using ComponentTypeID = uint8;
static TObjectPtr<UProfiler> ECSProfiler = NewObject<UProfiler>();


namespace ECSConstants
{
	inline constexpr uint8 MAX_COMPONENT_TYPES = 64;
	inline constexpr uint8 MAX_ENTITY = 32;
}


class ComponentTypeCounter
{
public:
	template <typename T>
	static ComponentTypeID GetTypeID()
	{
		static ComponentTypeID typeID = idCounter++;
		check(typeID < ECSConstants::MAX_COMPONENT_TYPES);
		return typeID;
	}

private:
	inline static std::atomic<ComponentTypeID> idCounter = 0;
};