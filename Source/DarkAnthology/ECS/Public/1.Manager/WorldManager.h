// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/ECSTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldManager.generated.h"


class URegisterUnregisterEntityMessage;
class USystemManager;
class UProfiler;
class UEntity;
class USystem;



UCLASS()
class DARKANTHOLOGY_API UWorldManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& collection) override;
	virtual void Deinitialize() override;
	void UpdateWorld(const float deltaTime);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USystemManager> systemManager;
	
	TLockFreePointerListUnordered<UEntity, ECSConstants::MAX_ENTITY> entitiesToAdd;
	TQueue<UEntity*> entitiesToRemove;

	inline void CreateObject();
	inline void Subscribe();
	
	void RegisterSystem(USystem* system = nullptr) const;
	void RegisterEntity(const URegisterUnregisterEntityMessage* message);
	void UnRegisterEntity(const URegisterUnregisterEntityMessage* message);

	
};