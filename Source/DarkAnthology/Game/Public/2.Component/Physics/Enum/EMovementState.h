#pragma once

UENUM(Blueprintable, Category = "Enum")
enum class EMovementState : uint8
{
	None,
	
	/**MoveState*/
		Place,
		Move,

	
	/**MoveType*/
		Walk,
		Run,

	
	/**MoveDirectionOne/MoveDirectionTwo*/
		Forward,
		Backward,
		Right,
		Left,


	/**Addition*/
		Jump,
		UnCrouching,
		Crouching,
		Crouch,
		Climbing,
		StartClimb,
		EndClimb
};
