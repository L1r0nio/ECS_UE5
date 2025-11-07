// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem3.h"


UTestSystem3::UTestSystem3()
{
	UTestSystem3::Initialize();
}

void UTestSystem3::Initialize()
{
	bNeedEntities = false;
	bSupportsParallelExecution = true;
	bRequiresMainThread = false;
	
	Super::Initialize();
}

void UTestSystem3::Update(const float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Emerald, "Test System 3");
}