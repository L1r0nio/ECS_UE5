// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "ECSTypes.h"
#include "GameFramework/Actor.h"     // Для AActor
#include "GameFramework/Character.h"
#include "UObject/Object.h"
#include "Entity.generated.h"



class UComponent;
class FObjectInitializer;


/**The basic entity. It is created as a separate object inside the actor.*/
UCLASS()
class DARKANTHOLOGY_API UEntity : public UObject
{
	GENERATED_BODY()

public:
	UEntity();
	virtual ~UEntity() override = default;

	void SetActive(const bool bNewActive);
	bool IsActive() const;
	
	void SetActor(AActor* actor);	
	AActor* GetActor() const;
	ACharacter* GetCharacter() const;
	
	bool HasComponents(ComponentMask mask) const;
	ComponentMask GetComponentMask() const;

	template <typename T>
	T* GetActorAs() const
	{
		static_assert(TIsDerivedFrom<T, AActor>::IsDerived, "T must derive from AActor");
		return Cast<T>(boundActor.Get());
	}
	
	template <typename T>
	bool HasComponent()
	{
		return (componentMask & (1ULL << T::StaticTypeID())) != 0;
	}
	
	template <typename T>
	T* GetComponent()
	{
		return Cast<T>(components[T::StaticTypeID()]);
	}
	
	template <typename TComponent>
	TComponent* AddComponent(const FObjectInitializer& objectInitializer, FName componentName)
	{
		static_assert(TIsDerivedFrom<TComponent, UComponent>::IsDerived, "T must derive from UComponent");
		
		if (HasComponent<TComponent>())
			return GetComponent<TComponent>();

		TComponent* component = objectInitializer.CreateDefaultSubobject<TComponent>(this, componentName);
		components[TComponent::StaticTypeID()] = component;
		componentMask |= component->GetMask();
		return component;
	}

	template <typename TComponent>
	TComponent* AddComponent()
	{
		static_assert(TIsDerivedFrom<TComponent, UComponent>::IsDerived, "T must derive from UComponent");
		
		if (HasComponent<TComponent>())
			return GetComponent<TComponent>();

		TComponent* component = NewObject<TComponent>(this);
		components[TComponent::StaticTypeID()] = component;
		componentMask |= component->GetMask();
		return component;
	}
	
	template <typename T>
	void RemoveComponent()
	{
		if (const ComponentTypeID typeID = T::StaticTypeID(); components[typeID])
		{
			components[typeID] = nullptr;
			componentMask &= ~(1ULL << typeID);
		}
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="ECS|Components")
	TArray<UComponent*> components;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="ECS|Ownew")
	TWeakObjectPtr<AActor> boundActor;
	
	ComponentMask componentMask;
	
	bool bIsActive;

private:
	void InitializeComponentArray();
};