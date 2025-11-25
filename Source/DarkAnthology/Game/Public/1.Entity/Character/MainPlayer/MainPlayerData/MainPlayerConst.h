#pragma once

/**Constants used for MainPlayer*/

namespace MainPlayerConst
{
	//------------------------------------------------------ DEBUG -----------------------------------------------------
	inline constexpr bool IS_SHOW_MOVEMENT_STATE = true;
	// -----------------------------------------------------------------------------------------------------------------

	

	//------------------------------------------------------ INPUT -----------------------------------------------------
	inline constexpr float MOUSE_SENSITIVITY = 0.4f;
	inline constexpr bool IS_DRIFT_LOOK = false;
	// -----------------------------------------------------------------------------------------------------------------

	

	//-------------------------------------------------- CAMERA --------------------------------------------------------	
	inline constexpr float CAMERA_ROTATION_LAG_SPEED = 28.6f;
	inline constexpr float FIELD_OF_VIEWS = 110.0f;

	inline constexpr float STANDARD_SPRING_ARM_LOCATION_Z = 50.0f;
	inline constexpr float CROUCH_SPRING_ARM_LOCATION_Z = 30.0f;
	//------------------------------------------------------------------------------------------------------------------

	

	//----------------------------------------------------- CAPSULE ----------------------------------------------------
	inline constexpr float STANDARD_CAPSULE_HALF_HEIGHT = 70.0f;
	inline constexpr float STANDARD_CAPSULE_RADIUS = 28.0f;
	// -----------------------------------------------------------------------------------------------------------------

	

	//---------------------------------------------------- MOVEMENT ----------------------------------------------------
	inline constexpr float INERTIA_UNPRESSING = 600.0f;
	inline constexpr float INERTIA_PRESSING = 1064.0f;

	inline constexpr float MOVE_X = 1.0f;
	inline constexpr float MOVE_Y = 1.0f;
	inline constexpr float WALK_DIAGONAL_SCALAR = 1.0f; //0.68f;
	inline constexpr float WALK_LINE_SCALAR = 1.0f;

	inline constexpr float CROUCH_SPEED = 100.0f;
	inline constexpr float WALK_SPEED = 210.0f;
	inline constexpr float SWIM_SPEED = 150.0f;
	inline constexpr float RUN_SPEED = 380.0f;

	inline constexpr float BRAKING_FRICTION_FRICTION = 5.55f;
	inline constexpr float BRAKING_FRICTION_FACTOR = 1.0f;
	inline constexpr float GRAVITY_SCALE = 1.84f;
	inline constexpr float BUOYANCY = 0.9f;
	// -----------------------------------------------------------------------------------------------------------------
}