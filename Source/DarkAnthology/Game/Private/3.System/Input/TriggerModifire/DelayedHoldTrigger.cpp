// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/Input/Trigger/DelayedHoldTrigger.h"

ETriggerState UDelayedHoldTrigger::UpdateState_Implementation(const UEnhancedPlayerInput* playerInput,
    FInputActionValue modifiedValue, float deltaTime)
{
    if (!playerInput)
        return ETriggerState::None;

    bool bIsPressed = modifiedValue.Get<bool>();

    if (bIsInCooldown)
        return ProcessCooldown(deltaTime);

    if (!bIsPressed && bWasPressed)
        return KeyReleased();

    if (bIsPressed && !bWasPressed)
        StartPressing();

    if (bIsPressed && bWasPressed)
        return ProcessDelayAndTrigger(deltaTime);

    bWasPressed = bIsPressed;
    return ETriggerState::None;
}

ETriggerState UDelayedHoldTrigger::KeyReleased()
{
    elapsedTime = 0.0f;
    bIsWaitingForDelay = false;
    bIsTriggerActive = false;
    bWasPressed = false;

    if (CooldownAfterRelease > 0.0f)
    {
        bIsInCooldown = true;
        elapsedTime = 0.0f;
    }
    
    return ETriggerState::None;
}

void UDelayedHoldTrigger::StartPressing()
{
    if (bIsInCooldown)
        return;

    bIsWaitingForDelay = true;
    bIsTriggerActive = false;
    elapsedTime = 0.0f;
    bWasPressed = true;
}

ETriggerState UDelayedHoldTrigger::ProcessDelayAndTrigger(const float deltaTime)
{
    elapsedTime += deltaTime;

    if (bIsWaitingForDelay && elapsedTime >= ActivationDelay)
    {
        bIsWaitingForDelay = false;
        bIsTriggerActive = true;
        elapsedTime = 0.0f;
        return ETriggerState::Triggered;
    }

    if (bIsTriggerActive && elapsedTime < TriggerDuration)
        return ETriggerState::Triggered;

    if (bIsTriggerActive && elapsedTime >= TriggerDuration)
    {
        bIsTriggerActive = false;
        return ETriggerState::None;
    }

    return ETriggerState::None;
}

ETriggerState UDelayedHoldTrigger::ProcessCooldown(const float deltaTime)
{
    elapsedTime += deltaTime;

    if (elapsedTime >= CooldownAfterRelease)
    {
        bIsInCooldown = false;
        elapsedTime = 0.0f;
    }

    return ETriggerState::None;
}
