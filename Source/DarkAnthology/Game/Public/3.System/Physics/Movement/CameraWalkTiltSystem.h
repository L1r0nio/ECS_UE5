// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "CameraWalkTiltSystem.generated.h"


struct FMovementState;
class UMainPlayerMovementComponent;
class USceneComponent;



/**For only MainPlayer*/
UCLASS()
class DARKANTHOLOGY_API UCameraWalkTiltSystem : public USystem
{
	GENERATED_BODY()

public:
	UCameraWalkTiltSystem();
	virtual ~UCameraWalkTiltSystem() override;
	virtual void RegisterEntity(UEntity* entity) override;
	virtual void UnregisterEntity(UEntity* entity) override;

protected:
	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual ComponentMask GetNeedComponents() const override;
	virtual bool GetAdditionalConditions(UEntity* entity) const override;
	virtual void Update(const float deltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> cameraWalkTiltPoint;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMainPlayerMovementComponent> mainPlayerMovementComponent;

	inline void CameraTilt(const float deltaTime) const;
	void SetRotation(const float newRotationX, const float deltaTime) const;
	inline void SetDefaultRotation(const float deltaTime) const;
	float GetTargetTiltAngle(const FMovementState& state) const;
	
	inline bool IsSystemReady() const;
};