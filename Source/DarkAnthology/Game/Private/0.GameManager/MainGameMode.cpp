// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/Game/Public/0.GameManager/MainGameMode.h"
#include "DarkAnthology/Game/Public/0.GameManager/GameData/GameConst.h"
#include "DarkAnthology/ECS/Public/1.Manager/WorldManager.h"
#include "Public/3.System/Input/MainPlayerController.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayer.h"


AMainGameMode::AMainGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = AMainPlayer::StaticClass();
	PlayerControllerClass = AMainPlayerController::StaticClass();
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();
	worldManager = GetGameInstance()->GetSubsystem<UWorldManager>();
	SetGameSettings();
}

void AMainGameMode::SetGameSettings() const
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality")))
		CVar->Set(GameConst::MotionBlurQuality);

	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS")))
		CVar->Set(GameConst::FPS);

	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync")))
		CVar->Set(GameConst::VSync);

	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSyncEditor")))
		CVar->Set(GameConst::VSync);
}



void AMainGameMode::Tick(const float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	if (worldManager)
		worldManager->UpdateWorld(deltaSeconds);
}
