#pragma once
#include "Public/2.Component/Physics/Enum/EMovementState.h"
#include "FMovementState.generated.h"



USTRUCT()
struct FMovementState
{
	GENERATED_BODY()

	/** Primary movement state: determines if character is moving or stationary */
	/**Place / Move*/
	UPROPERTY(VisibleAnywhere)
	EMovementState MoveState = EMovementState::Place;

	
	/** Movement type: walking, running, or none*/
	/**Walk / Run*/
	UPROPERTY(VisibleAnywhere)
	EMovementState MoveType = EMovementState::None;

	
	/** Primary movement direction (first axis)*/
	/**Forward / Backward*/
	UPROPERTY(VisibleAnywhere)
	EMovementState MoveOneDirection = EMovementState::None;


	/** Secondary movement direction (second axis, used for diagonals)*/
	/**Right / Left*/
	UPROPERTY(VisibleAnywhere)
	EMovementState MoveTwoDirection = EMovementState::None;


	/** Additional states: jumping, crouching, climbing, etc. */
	/**Jump / Crouch / Climb*/
	UPROPERTY(VisibleAnywhere)
	EMovementState Addition = EMovementState::None;


	void SetDefaultMovementState()
	{
		MoveState = EMovementState::Place;
		MoveType = EMovementState::None;
		MoveOneDirection = EMovementState::None;
		MoveTwoDirection = EMovementState::None;
	}
};