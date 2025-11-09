// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/1.Entity/Character/MainPlayer/MainPlayer.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnRegisterEntityMessage.h"


#pragma region SETTINGS

AMainPlayer::AMainPlayer(const FObjectInitializer& objectInitializer)
{
	CreateObject(objectInitializer);
	SetSetting();
}



void AMainPlayer::CreateObject(const FObjectInitializer& objectInitializer)
{
	Entity = objectInitializer.CreateDefaultSubobject<UEntity>
		(this, TEXT("MainPlayerEntity"));

	springArm = objectInitializer.CreateDefaultSubobject<USpringArmComponent>
		(this, TEXT("MainPlayerSpringArm"));

	camera = objectInitializer.CreateDefaultSubobject<UCameraComponent>
		(this, TEXT("MainPlayerCamera"));
}



void AMainPlayer::SetSetting()
{
	//GLOBAL FLAGS
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;


	//MOVEMENT
	GetCapsuleComponent()->ShapeColor = FColor::Black;
	GetCapsuleComponent()->InitCapsuleSize(MainPlayerConst::STANDARD_CAPSULE_RADIUS,
	                                       MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT);
	GetCharacterMovement()->MaxWalkSpeedCrouched = MainPlayerConst::CROUCH_SPEED;
	GetCharacterMovement()->MaxWalkSpeed = MainPlayerConst::WALK_SPEED;
	GetCharacterMovement()->MaxSwimSpeed = MainPlayerConst::SWIM_SPEED;


	//INERTIA
	GetCharacterMovement()->MaxAcceleration = MainPlayerConst::INERTIA_PRESSING;
	GetCharacterMovement()->BrakingDecelerationWalking = MainPlayerConst::INERTIA_UNPRESSING;


	//FACTOR
	GetCharacterMovement()->BrakingFrictionFactor = MainPlayerConst::BRAKING_FRICTION_FACTOR;
	GetCharacterMovement()->GroundFriction = MainPlayerConst::BRAKING_FRICTION_FRICTION;
	GetCharacterMovement()->GravityScale = MainPlayerConst::GRAVITY_SCALE;
	GetCharacterMovement()->Buoyancy = MainPlayerConst::BUOYANCY;


	//OBJECT
	springArm->SetupAttachment(RootComponent);
	springArm->SetRelativeLocation(FVector(0.0f, 0.0f, MainPlayerConst::STANDARD_SPRING_ARM_LOCATION_Z));
	springArm->bUsePawnControlRotation = true;
	springArm->bEnableCameraRotationLag = true;
	springArm->CameraRotationLagSpeed = MainPlayerConst::CAMERA_ROTATION_LAG_SPEED;
	springArm->TargetArmLength = 0.0f;

	camera->SetupAttachment(springArm);
	camera->AddRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	camera->bUsePawnControlRotation = false;
	camera->FieldOfView = MainPlayerConst::FIELD_OF_VIEWS;
}

#pragma endregion


void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (Entity)
	{
		SetComponent();
		
		if (UMessageBus* bus = UMessageBus::Get())
		{
			URegisterUnRegisterEntityMessage* message = NewObject<URegisterUnRegisterEntityMessage>(this);
			message->Setup(Entity);
			bus->Publish(message);
		}
	}
}

void AMainPlayer::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	if (Entity && UMessageBus::Get())
	{
		URegisterUnRegisterEntityMessage* message = NewObject<URegisterUnRegisterEntityMessage>(this);
		message->Setup(Entity, true);
		UMessageBus::Get()->Publish(message);
	}
    
	Super::EndPlay(endPlayReason);
}

void AMainPlayer::SetComponent()
{
	Entity->SetActor(this);

	Entity->AddComponent<UMainPlayerMovementComponent>();
	
	if (UEntityTypeComponent* entityTypeComponent = Entity->AddComponent<UEntityTypeComponent>())
		entityTypeComponent->EntityType = EEntityType::MainPlayer;
}
