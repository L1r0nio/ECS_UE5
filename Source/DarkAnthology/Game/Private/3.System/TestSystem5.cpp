// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem5.h"



UTestSystem5::UTestSystem5()
{
	UTestSystem5::Initialize();
}

void UTestSystem5::Initialize()
{
	bNeedEntities = false;
	bSupportsParallelExecution = true;
	bRequiresMainThread = false;
	
	Super::Initialize();
}

void UTestSystem5::Update(const float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Magenta, "Test System 5");
}
