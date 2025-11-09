#pragma once

/**Constants used for MainPlayer*/

namespace MainPlayerConst
{
	//------------------------------------------------------ DEBUG -----------------------------------------------------
	inline constexpr bool IS_SHOW_MOVMENT_STATE = true;
	inline constexpr bool IS_SHOW_CAPSULE_SCALE = false;
	inline constexpr bool IS_SHOW_DELTA_SECONDS = false;
	inline constexpr bool IS_SHOW_CAMERA_WALK_TILT = false;
	inline constexpr bool IS_SHOW_CAMERA_TILT = false;
	inline constexpr bool IS_SHOW_CLIMB_LOCATION = true;
	inline constexpr bool IS_SHOW_SYSTEM_UPDATE = true;

	inline constexpr bool IS_SHOW_UNCROUCH_BOX_COLLISION = false;
	inline constexpr bool IS_SHOW_CLIMB_COLLISION = false;
	inline constexpr bool IS_SHOW_CAMERA_TILT_SPHERE_COLLISION = false;


	bool IsCameraTiltSystemUpdate = false;
	bool IsCameraWalkTiltSystemUpdate = false;
	bool IsClimbSystemUpdate = false;
	bool IsMovmentCharacterSystemUpdate = false;
	// -----------------------------------------------------------------------------------------------------------------


	//------------------------------------------------------ INPUT ----------------------------------------------------
	inline constexpr float MOUSE_SENSITIVITY = 0.4f;
	inline constexpr bool IS_DRIFT_LOOK = false;
	// ----------------------------------------------------------------------------------------------------------------


	//-------------------------------------------------- CAMERA -----------------------------------------------------	
	inline constexpr float CAMERA_ROTATION_LAG_SPEED = 28.6f;
	inline constexpr float FIELD_OF_VIEWS = 110.0f;

	inline constexpr float STANDARD_SPRING_ARM_LOCATION_Z = 50.0f;
	inline constexpr float CROUCH_SPRING_ARM_LOCATION_Z = 30.0f;

	inline constexpr bool IS_CAMERA_TILT_ENABLE = true;
	inline constexpr float CAMERA_TILT_LOCATION_DEGREE = 50.0f;
	inline constexpr float CAMERA_TILT_ROTATION_DEGREE = 8.0f;
	inline constexpr float CAMERA_TILT_SPEED = 6.0f;
	inline constexpr float CAMERA_BLOCK_TILT_LINE_TRACE_LENGHT = CAMERA_TILT_LOCATION_DEGREE;

	inline constexpr bool IS_CAMERA_WALK_TILT_ENABLE = true;
	inline constexpr float CAMERA_WALK_TILT_DEGREE = 1.1f;
	inline constexpr float CAMERA_WALK_TILT_SPEED = 3.0f;
	//---------------------------------------------------------------------------------------------------------------


	//----------------------------------------------------- CAPSULE ---------------------------------------------------
	inline constexpr float STANDARD_CAPSULE_HALF_HEIGHT = 70.0f;
	inline constexpr float STANDARD_CAPSULE_RADIUS = 28.0f;

	inline constexpr float CROUCH_CAPSULE_HALF_HEIGHT = 28.0f;
	inline constexpr float CROUCH_CAPSULE_RADIUS = CROUCH_CAPSULE_HALF_HEIGHT;
	// ----------------------------------------------------------------------------------------------------------------


	//---------------------------------------------------- MOVEMENT ---------------------------------------------------
	inline constexpr float INERTIA_UNPRESSING = 600.0f;
	inline constexpr float INERTIA_PRESSING = 1064.0f;

	inline constexpr bool IS_CAN_CLIMB = true;
	inline constexpr float CLIMB_HEIGHT = STANDARD_CAPSULE_HALF_HEIGHT * 1.5f;
	inline constexpr float CLIMB_SPEED = 1.0f;

	inline constexpr float MOVE_X = 1.0f;
	inline constexpr float MOVE_Y = 1.0f;
	inline constexpr float WALK_DIAGONAL_SCALAR = 0.68f;

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