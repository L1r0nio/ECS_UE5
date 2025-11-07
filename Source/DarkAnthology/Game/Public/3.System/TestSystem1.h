// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "TestSystem1.generated.h"



UCLASS()
class DARKANTHOLOGY_API UTestSystem1 : public USystem
{
	GENERATED_BODY()

public:
	UTestSystem1();
	virtual void Initialize() override;
	virtual ComponentMask GetNeedComponents() const override; 
	virtual void Update(const float deltaTime) override;
	virtual void Update(UEntity* entity, const float deltaTime) override;
};