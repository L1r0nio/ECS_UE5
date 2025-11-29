// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "DelayedHoldTrigger.generated.h"



/**Completes the action of the trigger after a certain time. The trigger will not work again until the key is released*/
/**You can set after how much the trigger will begin to act after lowering the key*/
/**You can install cooldown during which the trigger is not a worker, no matter how much you press the buttons*/



UCLASS(NotBlueprintable, meta = (DisplayName = "Timed Trigger"))
class DARKANTHOLOGY_API UDelayedHoldTrigger : public UInputTrigger
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float TriggerDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ActivationDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CooldownAfterRelease = 0.0f;
	
protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* playerInput,
		FInputActionValue modifiedValue, float deltaTime) override;

private:
	float elapsedTime = 0.0f;
	bool bIsWaitingForDelay = false;
	bool bIsTriggerActive = false;
	bool bWasPressed = false;
	bool bIsInCooldown = false;

	ETriggerState KeyReleased();
	void StartPressing();
	ETriggerState ProcessDelayAndTrigger(const float deltaTime);
	ETriggerState ProcessCooldown(const float deltaTime);
};
