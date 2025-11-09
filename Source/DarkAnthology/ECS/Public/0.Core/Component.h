// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "ECSTypes.h"
#include "UObject/Object.h"
#include "Component.generated.h"


/**The basic component. We will inherit from this to create a component.*/

UCLASS(Abstract, BlueprintType, Blueprintable)
class DARKANTHOLOGY_API UComponent : public UObject
{
	GENERATED_BODY()

public:
	UComponent();
	virtual ~UComponent() override = default;
	
	virtual ComponentTypeID GetTypeID() const;
	ComponentMask GetMask() const;
};


/**A macro for registering a component in the system and assigning it a unique ID.
 *The macro is called after GENERATED_BODY()*/

#define DECLARE_TYPED_COMPONENT(ClassName) \
public: \
	static ComponentTypeID StaticTypeID() \
	{ \
		static ComponentTypeID TypeID = ComponentTypeCounter::GetTypeID<ClassName>(); \
		return TypeID; \
	} \
	virtual ComponentTypeID GetTypeID() const override \
	{ \
		return StaticTypeID(); \
	}