// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "ECSTypes.h"
#include "UObject/Object.h"
#include "System.generated.h"


class UEntity;


UCLASS(Abstract, BlueprintType, Blueprintable)
class DARKANTHOLOGY_API USystem : public UObject
{
	GENERATED_BODY()

public:
	bool bSupportsParallelExecution;
	bool bRequiresMainThread;
	
	USystem();
	virtual ~USystem() override;

	virtual void Initialize();
	virtual void Shutdown();

	virtual ComponentMask GetNeedComponents() const;
	virtual ComponentMask GetUnneededComponent() const;
	virtual bool GetAdditionalConditions(UEntity* entity) const;

	virtual void RegisterEntity(UEntity* entity);
	virtual void UnregisterEntity(UEntity* entity);
	virtual void StartUpdate(const float deltaTime);

	bool IsEnable() const;
	void SetEnable(const bool newStatus);

protected:
	bool bNeedEntities;
	TUniquePtr<TArray<UEntity*>> entities;

	bool ShouldProcessEntity(UEntity* entity) const;
	virtual void Update(UEntity* entity, const float deltaTime);
	virtual void Update(const float deltaTime);

private:
	mutable bool bMasksCached;
	bool bIsEnable;

	mutable ComponentMask cachedRequiredMask;
	mutable ComponentMask cachedExcludedMask;

	void CacheMasks() const;
};