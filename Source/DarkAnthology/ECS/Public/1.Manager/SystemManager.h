// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "SystemManager.generated.h"

USTRUCT()
struct FParallelBatch
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<USystem*> Systems;
	
	ComponentMask CombinedMask = 0;

	bool CanAddSystem(USystem* system) const
	{
		if (!system || Systems.Contains(system))
			return false;

		return (system->GetNeedComponents() & CombinedMask) == 0;
	}

	void AddSystem(USystem* system)
	{
		Systems.Add(system);
		CombinedMask |= system->GetNeedComponents();
	}
};



UCLASS()
class DARKANTHOLOGY_API USystemManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TArray<USystem*> RegisteredSystems;
	
	USystemManager();
	virtual ~USystemManager() override;
	
	void Initialize();
	void Shutdown();
	
	template <typename TSystem>
	void RegisterSystem()
	{
		static_assert(TIsDerivedFrom<TSystem, USystem>::IsDerived, "T must derive from UComponent");

		TSystem* system = NewObject<TSystem>(this);

		if (!system)
			return;

		RegisteredSystems.Add(system);
		bNeedsRebuild = true;
		
		UE_LOG(LogTemp, Warning, TEXT("System %s registered"), 
		   *system->GetClass()->GetName());
	}

	void RegisterEntity(UEntity* entity);
	void UnregisterEntity(UEntity* entity);
	void RegisterSystem(USystem* system);
	void UnregisterSystem(USystem* system);
	void UpdateSystem(const float deltaTime);

private:
	TArray<FParallelBatch> parallelBatches;
	
	bool bParallelExecutionEnabled;
	int32 maxParallelTasks;
	bool bNeedsRebuild;
	
	void RebuildBatches();
	void ExecuteBatch(const FParallelBatch& batch, const float deltaTime);
};
