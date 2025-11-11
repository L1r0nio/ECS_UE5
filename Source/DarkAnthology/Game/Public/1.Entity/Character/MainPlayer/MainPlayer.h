// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"



class UEntity;



UCLASS()
class DARKANTHOLOGY_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayer(const FObjectInitializer& objectInitializer);
	UEntity* GetEntity() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UEntity> entity;
	
	inline void CreateObject(const FObjectInitializer& objectInitializer);
	inline void SetSettings();
	inline void SetComponent();
	
	inline void SendMessage();
};