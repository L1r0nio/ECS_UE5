// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem2.h"


UTestSystem2::UTestSystem2()
{
	UTestSystem2::Initialize();
}

void UTestSystem2::Initialize()
{
	
	bNeedEntities = false;
	bSupportsParallelExecution = true;
	bRequiresMainThread = false;
	
	Super::Initialize();
}

void UTestSystem2::Update(const float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Test System 2");
}
