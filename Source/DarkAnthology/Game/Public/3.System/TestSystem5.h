// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "TestSystem5.generated.h"



UCLASS()
class DARKANTHOLOGY_API UTestSystem5 : public USystem
{
	GENERATED_BODY()
	
public:
	UTestSystem5();
	virtual void Initialize() override;
	virtual void Update(const float deltaTime) override;
};
