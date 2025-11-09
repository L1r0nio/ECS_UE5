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
	UMovementSystem();
	virtual void Initialize() override;
	virtual ComponentMask GetNeedComponents() const override;
	virtual bool GetAdditionalConditions(UEntity* entity) const override;
	virtual void Update(UEntity* entity, const float deltaTime) override;
};
