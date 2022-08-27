// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/MeleeTemplateCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/InputComponent.h"
#include "Components/LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Props/WeaponBase.h"

//////////////////////////////////////////////////////////////////////////
// AMeleeTemplateCharacter



AMeleeTemplateCharacter::AMeleeTemplateCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input




void AMeleeTemplateCharacter::TryAttack_Implementation()
{
	switch (CharacterState)
	{
		case Idle:
			SetAndStartAttack();
		break;

		case Attacking:
			CombatComponent->bInputBuffer = true;
		break;
			
		default: break;
	}
}

void AMeleeTemplateCharacter::ActivateSkill(UAttackAsset* SkillSlot)
{
	UAttackAsset* Skill = CombatComponent->TrySkill(SkillSlot);
	if (Skill)
	{
		LockOnComponent->RotateOwnerToTarget();
		StartAttack(Skill->Animation);
	}
}

void AMeleeTemplateCharacter::TrySkillSlot1()
{
	if (!SkillSlot1)
		return;
	
	ActivateSkill(SkillSlot1);
}

void AMeleeTemplateCharacter::TrySkillSlot2()
{
	if (!SkillSlot2)
		return;
	ActivateSkill(SkillSlot2);
}

void AMeleeTemplateCharacter::TrySkillSlot3()
{
	if (!SkillSlot3)
		return;

	ActivateSkill(SkillSlot3);
}

void AMeleeTemplateCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMeleeTemplateCharacter::TryAttack);
	PlayerInputComponent->BindAction("SkillSlot1", IE_Pressed, this, &AMeleeTemplateCharacter::TrySkillSlot1);
	PlayerInputComponent->BindAction("SkillSlot2", IE_Pressed, this, &AMeleeTemplateCharacter::TrySkillSlot2);
	PlayerInputComponent->BindAction("SkillSlot3", IE_Pressed, this, &AMeleeTemplateCharacter::TrySkillSlot3);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMeleeTemplateCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMeleeTemplateCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMeleeTemplateCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMeleeTemplateCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMeleeTemplateCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMeleeTemplateCharacter::TouchStopped);
}

void AMeleeTemplateCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMeleeTemplateCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMeleeTemplateCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeTemplateCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeTemplateCharacter::MoveForward(float Value)
{
	switch (CharacterState)
	{
		case Idle:
			if ((Controller != nullptr) && (Value != 0.0f))
			{
				StopAnimMontage();
				Execute_EndAttack(this);
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, Value);
			}
		break;
		case Attacking: default: break;
	}
	
}

void AMeleeTemplateCharacter::MoveRight(float Value)
{
	switch (CharacterState)
	{
		case Idle:
			if ( (Controller != nullptr) && (Value != 0.0f) )
			{
				StopAnimMontage();
				Execute_EndAttack(this);
				
				// find out which way is right
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
	
				// get right vector 
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				// add movement in that direction
				AddMovementInput(Direction, Value);
			}
		break;
	case Attacking: default: break;
	}
	
}
