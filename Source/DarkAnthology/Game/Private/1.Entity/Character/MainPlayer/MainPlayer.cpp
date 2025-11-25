// L1 Game copyright. 2025. All rights reserved, probably :)
#include "Public/1.Entity/Character/MainPlayer/MainPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DarkAnthology/ECS/Public/0.Core/Entity.h"
#include "DarkAnthology/ECS/Public/0.Core/Message.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/1.Entity/Character/MainPlayer/MainPlayerData/MainPlayerConst.h"
#include "Public/2.Component/Physics/MainPlayerMovementComponent.h"
#include "Public/2.Component/Utility/Carriers/EntityTypeComponent.h"
#include "Public/4.Message/EntityLife/RegisterUnregisterEntityMessage.h"



#pragma region SETTINGS

AMainPlayer::AMainPlayer(const FObjectInitializer& objectInitializer)
{
	CreateObject(objectInitializer);
	SetSettings();
}

void AMainPlayer::CreateObject(const FObjectInitializer& objectInitializer)
{
	entity = objectInitializer.CreateDefaultSubobject<UEntity>
		(this, TEXT("MainPlayerEntity"));
	
	springArm = objectInitializer.CreateDefaultSubobject<USpringArmComponent>
		(this, TEXT("MainPlayerSpringArm"));
	
	camera = objectInitializer.CreateDefaultSubobject<UCameraComponent>
		(this, TEXT("MainPlayerCamera"));
}

void AMainPlayer::SetSettings()
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
	GetCapsuleComponent()->InitCapsuleSize(MainPlayerConst::STANDARD_CAPSULE_RADIUS, MainPlayerConst::STANDARD_CAPSULE_HALF_HEIGHT);
	GetCharacterMovement()->MaxWalkSpeedCrouched = MainPlayerConst::CROUCH_SPEED;
	GetCharacterMovement()->MaxWalkSpeed = MainPlayerConst::WALK_SPEED;
	GetCharacterMovement()->MaxSwimSpeed = MainPlayerConst::SWIM_SPEED;
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

void AMainPlayer::SetComponent()
{
	entity->SetActor(this);

	UEntityTypeComponent* entityTypeComponent = entity->AddComponent<UEntityTypeComponent>();
	entityTypeComponent->EntityType = EEntityType::MainPlayer;

	entity->AddComponent<UMainPlayerMovementComponent>();
}

#pragma endregion



#pragma region SYSTEM

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (entity)
	{
		SetComponent();
		SendMessage();
	}
}

void AMainPlayer::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	if (entity != nullptr)
	{
		SendMessage(true);
		entity = nullptr;
	}
}

void AMainPlayer::Tick(const float deltaTime)
{
	Super::Tick(deltaTime);

	if (MainPlayerConst::IS_SHOW_MOVEMENT_STATE)
	{
		if (UMainPlayerMovementComponent* movementComponent = entity->GetComponent<UMainPlayerMovementComponent>())
		{
			if (movementComponent->bIsEnableMovement)
			{
				const FMovementState& state = movementComponent->MovementStates;
	            
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT(""));
            				
            				
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("----------------------------"));

				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
				FString::Printf(TEXT(" Speed: %f"), GetCharacterMovement()->MaxWalkSpeed));
				
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
				FString::Printf(TEXT(" LookAxis: %s"), *entity->GetComponent<UMainPlayerMovementComponent>()->LookAxis.ToString()));
				
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
				FString::Printf(TEXT(" Add: %s"), *UEnum::GetValueAsString(state.Addition)));
	            
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
				FString::Printf(TEXT(" Dir2: %s"), *UEnum::GetValueAsString(state.MoveTwoDirection)));
	            
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
				FString::Printf(TEXT(" Dir1: %s"), *UEnum::GetValueAsString(state.MoveOneDirection)));
	            
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
				FString::Printf(TEXT(" Type: %s"), *UEnum::GetValueAsString(state.MoveType)));
            				
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, 
				FString::Printf(TEXT(" State: %s"), *UEnum::GetValueAsString(state.MoveState)));
            			
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Silver, TEXT("---------MOV STATE--------"));
			}
		}
	}
	
}

#pragma endregion



#pragma region ADDITIONAL METOD

void AMainPlayer::SendMessage(const bool bIsUnregister)
{
	if (UMessageBus* bus = UMessageBus::Get())
	{
		URegisterUnregisterEntityMessage* message = NewObject<URegisterUnregisterEntityMessage>(this);
		message->Setup(entity, bIsUnregister);
		bus->Publish(message);
	}
}

UEntity* AMainPlayer::GetEntity() const
{
	return entity;
}

#pragma endregion