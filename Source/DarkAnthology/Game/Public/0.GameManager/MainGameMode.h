// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"



class UWorldManager;



UCLASS()
class DARKANTHOLOGY_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainGameMode();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float deltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere);
	UWorldManager* worldManager;

	inline void SetGameSettings() const;
	inline void CreateObject();
};