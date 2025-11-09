// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Message.generated.h"



struct FMessageSubscription
{
	FDelegateHandle Handle;
	TWeakObjectPtr<UObject> Subscriber;
	FString MessageType;

	bool IsValid() const
	{
		return Handle.IsValid() && Subscriber.IsValid();
	}
};



/**The base class for messages. We will inherit from this if we want to create a message. After creating the message, you need to call Initialize() instead.*/
UCLASS(Abstract, BlueprintType)
class DARKANTHOLOGY_API UMessage : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Messages)
	TObjectPtr<UObject> Sender;
	
	float Timestamp = 0.0f;

	void Initialize(UObject* sender = nullptr)
	{
		Sender = sender;
		Timestamp = FPlatformTime::Seconds();
	}
};



DECLARE_DELEGATE_OneParam(FMessageDelegate, const UMessage*);
DECLARE_MULTICAST_DELEGATE_OneParam(FMessageMulticastDelegate, const UMessage*);


/**Forwarding messages between systems.*/
UCLASS()
class DARKANTHOLOGY_API UMessageBus : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UMessageBus() override;
	
	static UMessageBus* Get();
	static void Initialize();
	static void Shutdown();
	
	template <typename TMessage>
	FMessageSubscription Subscribe(UObject* subscriber, TFunction<void(const TMessage*)> handler)
	{
		static_assert(TIsDerivedFrom<TMessage, UMessage>::IsDerived, "TMessage must derive from UMessage");

		FString messageType = TMessage::StaticClass()->GetName();
		FMessageMulticastDelegate& delegate = messageHandlers.FindOrAdd(messageType);

		FMessageSubscription subscription;
		subscription.MessageType = messageType;
		subscription.Subscriber = subscriber;
		subscription.Handle = delegate.AddLambda([handler](const UMessage* message)
		{
			if (const TMessage* typedMessage = Cast<const TMessage>(message))
				handler(typedMessage);
		});

		activeSubscriptions.Add(subscription);
		return subscription;
	}

	template <typename TMessage>
	void Publish(TMessage* message)
	{
		static_assert(TIsDerivedFrom<TMessage, UMessage>::IsDerived, "TMessage must derive from UMessage");
		
		if (!message)
			return;
		
		const FString messageType = message->GetClass()->GetName();

		if (bDeferredProcessing)
		{
			messageQueue.Enqueue(message);
		}
		
		else
		{
			ProcessMessage(message);
		}


		messageStats.TotalPublished++;
		++messageTypeStats.FindOrAdd(messageType);
	}

	void Unsubscribe(const FMessageSubscription& subscription);
	void UnsubscribeAll(const UObject* subscriber);
	
	void ProcessMessageQueue();
	void SetDeferredProcessing(bool bDeferred);
	bool IsDeferredProcessing() const;


#pragma region Statistics
	
	struct FMessageStats
	{
		int32 TotalPublished = 0;
		int32 TotalProcessed = 0;
		int32 QueuedMessages = 0;
	};

	const FMessageStats& GetStatistics() const;
	void PrintStatistics() const;
	void ResetStatistics();
	
#pragma endregion
	
private:
	inline static UMessageBus* instance = nullptr;
	
	TQueue<UMessage*> messageQueue;
	bool bDeferredProcessing = false;
	
	TMap<FString, FMessageMulticastDelegate> messageHandlers;
	TArray<FMessageSubscription> activeSubscriptions;
	
	void ProcessMessage(const UMessage* message);
	void CleanupInvalidSubscriptions();


#pragma region Statistics

	FMessageStats messageStats;
	TMap<FString, int32> messageTypeStats;

#pragma endregion
};