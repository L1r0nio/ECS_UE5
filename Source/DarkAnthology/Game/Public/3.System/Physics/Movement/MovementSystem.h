// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "MovementSystem.generated.h"



class UEntity;



/**For only MainPlayer*/
UCLASS()
class DARKANTHOLOGY_API UMovementSystem : public USystem
{
	GENERATED_BODY()

	
public:
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
	TObjectPtr<UEntity> mainPlayer;
};
