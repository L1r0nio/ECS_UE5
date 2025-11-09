// L1 Game copyright. 2025. All rights reserved, probably :)
#include "DarkAnthology/ECS/Public/0.Core/Message.h"



UMessageBus::~UMessageBus()
{
	Shutdown();
}


UMessageBus* UMessageBus::Get()
{
	if (!instance)
		Initialize();

	return instance;
}

void UMessageBus::Initialize()
{
	if (!instance)
	{
		instance = NewObject<UMessageBus>(GetTransientPackage(), UMessageBus::StaticClass());
		instance->AddToRoot();

		UE_LOG(LogTemp, Log, TEXT("MessageBus initialized"));
	}
}

void UMessageBus::Shutdown()
{
	if (instance)
	{
		instance->activeSubscriptions.Empty();
		instance->messageHandlers.Empty();

		UMessage* message = nullptr;
		while (instance->messageQueue.Dequeue(message)){}
		
		instance->RemoveFromRoot();
		instance = nullptr;
		
		UE_LOG(LogTemp, Log, TEXT("MessageBus shutdown"));
	}
}



void UMessageBus::Unsubscribe(const FMessageSubscription& subscription)
{
	if (!subscription.IsValid())
		return;

	if (FMessageMulticastDelegate* delegate = messageHandlers.Find(subscription.MessageType))
		delegate->Remove(subscription.Handle);

	activeSubscriptions.RemoveAll([&](const FMessageSubscription& sub)
	{
		return sub.Handle == subscription.Handle;
	});
}

void UMessageBus::UnsubscribeAll(const UObject* subscriber)
{
	if (!subscriber)
		return;

	TArray<FMessageSubscription> toRemove;

	for (const FMessageSubscription& sub : activeSubscriptions)
	{
		if (sub.Subscriber == subscriber)
			toRemove.Add(sub);
	}

	for (const FMessageSubscription& sub : toRemove)
	{
		Unsubscribe(sub);
	}
}

void UMessageBus::ProcessMessageQueue()
{
	UMessage* message = nullptr;
	int32 processedCount = 0;
	constexpr int32 maxProcessPerFrame = 100;

	while (messageQueue.Dequeue(message) && processedCount < maxProcessPerFrame)
	{
		ProcessMessage(message);
		processedCount++;
	}

	messageStats.QueuedMessages = 0;
}



void UMessageBus::ProcessMessage(const UMessage* message)
{
	if (!message)
		return;

	const FString messageType = message->GetClass()->GetName();

	if (const FMessageMulticastDelegate* delegate = messageHandlers.Find(messageType))
		delegate->Broadcast(message);

	messageStats.TotalProcessed++;
}

void UMessageBus::CleanupInvalidSubscriptions()
{
	activeSubscriptions.RemoveAll([this](const FMessageSubscription& Sub)
	{
		if (!Sub.IsValid())
		{
			if (FMessageMulticastDelegate* Delegate = messageHandlers.Find(Sub.MessageType))
				Delegate->Remove(Sub.Handle);
			
			return true;
		}
		
		return false;
	});
}

void UMessageBus::SetDeferredProcessing(bool bDeferred)
{
	bDeferredProcessing = bDeferred;
}

bool UMessageBus::IsDeferredProcessing() const
{
	return bDeferredProcessing;
}



#pragma region Statistics

const UMessageBus::FMessageStats& UMessageBus::GetStatistics() const
{
	return messageStats;
}

void UMessageBus::PrintStatistics() const
{
	UE_LOG(LogTemp, Display, TEXT("=== MessageBus Statistics ==="));
	UE_LOG(LogTemp, Display, TEXT("Total Published: %d"), messageStats.TotalPublished);
	UE_LOG(LogTemp, Display, TEXT("Total Processed: %d"), messageStats.TotalProcessed);
	UE_LOG(LogTemp, Display, TEXT("Active Subscriptions: %d"), activeSubscriptions.Num());
	UE_LOG(LogTemp, Display, TEXT("Message Types: %d"), messageHandlers.Num());
	
	UE_LOG(LogTemp, Display, TEXT("--- Message Type Breakdown ---"));
	
	for (const auto& Pair : messageTypeStats)
		UE_LOG(LogTemp, Display, TEXT("  %s: %d"), *Pair.Key, Pair.Value);
}

void UMessageBus::ResetStatistics()
{
	messageStats = FMessageStats();
	messageTypeStats.Empty();
}

#pragma endregion