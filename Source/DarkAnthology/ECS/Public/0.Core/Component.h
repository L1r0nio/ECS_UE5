// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "ECSTypes.h"
#include "UObject/Object.h"
#include "Component.generated.h"



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
