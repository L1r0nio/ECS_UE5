// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "RegisterUnRegisterEntityMessage.generated.h"


class UEntity;



UCLASS()
class DARKANTHOLOGY_API URegisterUnRegisterEntityMessage : public UMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEntity> Entity;
	
	bool UnRegisterEntity = false;

	void Setup(UEntity* entity)
	{
		Entity = entity;
		Initialize();
	}

	void Setup(UEntity* entity, const bool unRegister)
	{
		Entity = entity;
		UnRegisterEntity = unRegister;
		Initialize();
	}
};