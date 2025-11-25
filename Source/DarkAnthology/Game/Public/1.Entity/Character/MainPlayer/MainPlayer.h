// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"



class UEntity;
class UCameraComponent;
class USpringArmComponent;



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
	virtual void Tick(float deltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ECS|Entity")
	TObjectPtr<UEntity> entity;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> springArm;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> camera;
	
	inline void CreateObject(const FObjectInitializer& objectInitializer);
	inline void SetSettings();
	inline void SetComponent();
	
	inline void SendMessage(const bool bIsUnregister = false);
};