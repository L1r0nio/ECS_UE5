// L1 Game copyright. 2025. All rights reserved , probably :)
#include "DarkAnthology/ECS/Public/1.Manager/SystemManager.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"



bool FParallelBatch::CanAddSystem(USystem* system) const
{
	if (!system || Systems.Contains(system))
		return false;

	return (system->GetNeedComponents() & CombinedMask) == 0;
}

void FParallelBatch::AddSystem(USystem* system)
{
	Systems.Add(system);
	CombinedMask |= system->GetNeedComponents();
}



USystemManager::USystemManager()
{
	bParallelExecutionEnabled = true;
	bNeedsRebuild = true;
	maxParallelTasks = 4;
}

USystemManager::~USystemManager()
{
	Shutdown();
}

void USystemManager::Initialize()
{
	RegisteredSystems.Empty();
	parallelBatches.Empty();
	bNeedsRebuild = true;
    
	UE_LOG(LogTemp, Log, TEXT("SystemManager initialized"));
}

void USystemManager::Shutdown()
{
	RegisteredSystems.Empty();
	parallelBatches.Empty();
	
	UE_LOG(LogTemp, Log, TEXT("SystemManager shutdown"));
}



void USystemManager::RegisterEntity(UEntity* entity)
{
	if (entity)
	{
		for (USystem* system : RegisteredSystems)
			system->RegisterEntity(entity);
	}
}

void USystemManager::UnregisterEntity(UEntity* entity)
{
	if (entity)
	{
		for (USystem* system : RegisteredSystems)
			system->UnregisterEntity(entity);
	}
		
}

void USystemManager::RegisterSystem(USystem* system)
{
	if (!system || RegisteredSystems.Contains(system))
		return;

	RegisteredSystems.Add(system);
	bNeedsRebuild = true;

	UE_LOG(LogTemp, Warning, TEXT("System %s registered"), 
		   *system->GetClass()->GetName());
}

void USystemManager::UnregisterSystem(USystem* system)
{
	if (!system)
		return;

	RegisteredSystems.Remove(system);
	bNeedsRebuild = true;

	UE_LOG(LogTemp, Error, TEXT("System %s unregistered"), 
		   *system->GetClass()->GetName());
}

void USystemManager::UpdateSystem(const float deltaTime)
{
	if (bNeedsRebuild)
	{
		RebuildBatches();
		bNeedsRebuild = false;
	}

	for (const FParallelBatch& batch : parallelBatches)
		ExecuteBatch(batch, deltaTime);

	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("ECSProfilerReport.txt");
	ECSProfiler->SaveReport(FilePath);
}



void USystemManager::RebuildBatches()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RebuildBatches);
	
	parallelBatches.Empty();

	TArray<USystem*> systemsToProcess  = RegisteredSystems;

	while (systemsToProcess.Num() > 0)
	{
		FParallelBatch newBatch;

		for (int32 i = systemsToProcess.Num() - 1; i>= 0; --i)
		{
			USystem* system = systemsToProcess[i];

			if (!system || !system->IsEnable())
			{
				systemsToProcess.RemoveAt(i);
				continue;
			}

			if (system->bRequiresMainThread)
			{
				if (newBatch.Systems.Num() == 0)
				{
					newBatch.AddSystem(system);
					systemsToProcess.RemoveAt(i);
					break;
				}
			}
			
			else if (system->bSupportsParallelExecution && newBatch.CanAddSystem(system))
			{
				newBatch.AddSystem(system);
				systemsToProcess.RemoveAt(i);
			}
		}

		if (newBatch.Systems.Num() > 0)
			parallelBatches.Add(newBatch);
	}

	UE_LOG(LogTemp, Warning, TEXT("Created %d batches from %d systems:"), 
		   parallelBatches.Num(), RegisteredSystems.Num());

	for (int32 i = 0; i < parallelBatches.Num(); ++i)
	{
		const FParallelBatch& batch = parallelBatches[i];
		FString systemNames = "";

		for (USystem* system : batch.Systems)
			systemNames += system->GetClass()->GetName() + TEXT(", ");

		UE_LOG(LogTemp, Warning, TEXT("  Batch %d (%d systems): %s"), 
			   i, batch.Systems.Num(), *systemNames);
	}
}

void USystemManager::ExecuteBatch(const FParallelBatch& batch, const float deltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(ExecuteBatch);
	
	if (batch.Systems.Num() == 0)
		return;

	if (batch.Systems.Num() == 1)
	{
		if (USystem* system = batch.Systems[0])
		{
			if (system->IsEnable())
			{
				PROFILE_ECS_SCOPE(ECSProfiler, "Main");
				system->StartUpdate(deltaTime);
			}
		}
		
		return;
	}

	#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
		FString::Printf(TEXT("Executing %d systems in parallel"), batch.Systems.Num()));
	#endif

	PROFILE_ECS_SCOPE(ECSProfiler, "Parallel");
	ParallelFor(batch.Systems.Num(), [&batch, deltaTime](const int32 index)
	{
		if (batch.Systems.IsValidIndex(index))
		{
			if (USystem* system = batch.Systems[index])
			{
				if (IsValid(system) && system->IsEnable())
					system->StartUpdate(deltaTime);
			}
		}
		
	}, EParallelForFlags::None);

	ECSProfiler->DrawRealtimeStats(GetWorld());
}