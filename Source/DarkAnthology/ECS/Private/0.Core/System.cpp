// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/0.Core/System.h"



USystem::USystem()
{
	SetFlags(RF_NoFlags);
	bIsEnable = true;
	bMasksCached = false;
	bNeedEntities = true;
	bRequiresMainThread = true;
	bSupportsParallelExecution = false;
	cachedRequiredMask = 0;
	cachedExcludedMask = 0;
}

USystem::~USystem()
{
	USystem::Shutdown();
}

void USystem::Initialize()
{
	CacheMasks();

	if (bIsEnable)
	{
		entities = MakeUnique<TArray<UEntity*>>();
	}
	
	else
	{
		entities.Reset();
	}
}

void USystem::Shutdown()
{
	if (entities)
	{
		entities->Empty();
		entities.Reset();
	}
	
	cachedRequiredMask = 0;
	cachedExcludedMask = 0;
}



bool USystem::IsEnable() const
{
	return bIsEnable;
}

void USystem::SetEnable(const bool newStatus)
{
	bIsEnable = newStatus;
}



ComponentMask USystem::GetNeedComponents() const
{
	return 0;
}

ComponentMask USystem::GetUnneededComponent() const
{
	return 0;
}



bool USystem::ShouldProcessEntity(UEntity* entity) const
{
	if (!entity || !entity->IsActive())
		return false;

	if (!bMasksCached)
		CacheMasks();

	if (cachedRequiredMask != 0 && !entity->HasComponents(cachedRequiredMask))
		return false;

	if (cachedExcludedMask != 0 && (entity->GetComponentMask() & cachedExcludedMask) != 0)
		return false;
	

	return GetAdditionalConditions(entity);
}

bool USystem::GetAdditionalConditions(UEntity* entity) const
{
	return true;
}

void USystem::RegisterEntity(UEntity* entity)
{
	if (!bNeedEntities || !entities || !entity || !ShouldProcessEntity(entity))
		return;
	
	entities->Add(entity);
}

void USystem::UnregisterEntity(UEntity* entity)
{
	if (!bNeedEntities || !entities || !entity)
		return;

	entities->Remove(entity);
}

void USystem::StartUpdate(const float deltaTime)
{
	if (!bIsEnable)
		return;
	
	if (bNeedEntities && entities)
	{
		for (UEntity* entity : *entities)
		{
			if (entity && entity->IsActive())
				Update(entity, deltaTime);
		}
	}
	
	else
	{
		Update(deltaTime);
	}
}	

void USystem::Update(UEntity* entity, const float deltaTime){}

void USystem::Update(const float deltaTime) {}



void USystem::CacheMasks() const
{
	cachedRequiredMask = GetNeedComponents();
	cachedExcludedMask = GetUnneededComponent();
	bMasksCached = true;
}