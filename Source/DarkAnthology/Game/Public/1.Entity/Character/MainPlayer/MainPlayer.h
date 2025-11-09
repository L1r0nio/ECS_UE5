// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"



class UCameraComponent;
class USpringArmComponent;
class UEntity;



UCLASS()
class DARKANTHOLOGY_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayer(const FObjectInitializer& objectInitializer);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEntity> Entity;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> springArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> camera;
	
	void CreateObject(const FObjectInitializer& objectInitializer);
	void SetSetting();
	void SetComponent();
};