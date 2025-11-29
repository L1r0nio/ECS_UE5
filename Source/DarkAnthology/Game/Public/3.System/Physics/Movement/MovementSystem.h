// L1 Game copyright. 2025. All rights reserved, probably :)
#pragma once
#include "CoreMinimal.h"
#include "DarkAnthology/ECS/Public/0.Core/System.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/Enum/EMovementState.h"
#include "MovementSystem.generated.h"



class UMainPlayerMovementComponent;
class UCapsuleComponent;
struct FMovementState;
class UEntity;



/**For only MainPlayer*/
UCLASS()
class DARKANTHOLOGY_API UMovementSystem : public USystem
{
	GENERATED_BODY()

public:
	UMovementSystem();
	virtual ~UMovementSystem() override;
	virtual void RegisterEntity(UEntity* entity) override;
	virtual void UnregisterEntity(UEntity* entity) override;
	

protected:
	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual ComponentMask GetNeedComponents() const override;
	virtual bool GetAdditionalConditions(UEntity* entity) const override;
	virtual void Update(const float deltaTime) override;


private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEntity> mainPlayerEntity;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCurveFloat> crouchCurveAlpha;

	mutable float crouchCurrentTime;
	mutable float crouchCurveAllTime;
	
	static constexpr std::array<std::pair<EMovementState, float>, 5> speedTable =
	{{
		{EMovementState::Place, MainPlayerConst::WALK_SPEED},
		{EMovementState::Walk, MainPlayerConst::WALK_SPEED},
		{EMovementState::Crouching, MainPlayerConst::CROUCH_SPEED},
		{EMovementState::UnCrouching, MainPlayerConst::WALK_SPEED},
		{EMovementState::Crouch, MainPlayerConst::CROUCH_SPEED}
	}};

	
	void Walk(const UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const;
	void Look(UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer) const;
	void Addition(UMainPlayerMovementComponent& movementComponent, ACharacter& mainPlayer, float deltaTime) const;

	void Jump(ACharacter& mainPlayer) const;
	void Crouch(const UMainPlayerMovementComponent& movementComponent, EMovementState& addition, const ACharacter& mainPlayer, float
	            deltaTime) const;
	
	FVector2D GetLocalDirection(const FMovementState& movementState, bool& bIsDiagonal) const;
	FVector CalculateWorldDirection(const FVector2D& localDirection, const ACharacter& mainPlayer) const;
	
	inline void ProcessCrouchTransition(UCapsuleComponent& capsuleComponent, USpringArmComponent& springArmComponent,
		EMovementState& addition, const bool bIsCrouching, const float deltaTime) const;
	float CurveInterpolateFloatValue(float oldValue, float newValue) const;
	
	void UpdateMovementSpeed(const EMovementState state, const ACharacter& mainPlayer) const;
	
	inline bool IsSystemReady() const;
};
