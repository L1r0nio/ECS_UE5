// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/3.System/TestSystem4.h"

UTestSystem4::UTestSystem4()
{
	UTestSystem4::Initialize();
}

void UTestSystem4::Initialize()
{
	bNeedEntities = false;
	bSupportsParallelExecution = true;
	bRequiresMainThread = false;
	
	Super::Initialize();
}

void UTestSystem4::Update(float deltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "Test System 4");
}
