// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/InputComponent.h"
#include "AttackAsset.h"
#include "MotionWarpingComponent.h"
#include "Components/LockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Props/WeaponBase.h"

//////////////////////////////////////////////////////////////////////////
// AMeleeTemplateCharacter



APlayerCharacter::APlayerCharacter()
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

	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("Lock-On Component"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));

	
}

//////////////////////////////////////////////////////////////////////////
// Input




void APlayerCharacter::TryAttack_Implementation()
{
	switch (CharacterState)
	{
		case Idle:
			SetAndStartAttack();
			
		break;

		case Attacking: case Dodging:
			CombatComponent->bInputBuffer = true;
		break;
			
		default: break;
	}
}

void APlayerCharacter::ActivateSkill(UAttackAsset* SkillSlot)
{
	UAttackAsset* Skill = CombatComponent->TrySkill(SkillSlot);
	if (Skill)
	{
		if (GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance)
		{
			LockOnComponent->RotateOwnerToTarget();
		}
		
		StartAttack(Skill->Animation);
	}
}

void APlayerCharacter::TrySkillSlot1()
{
	if (!SkillSlot1)
		return;


	if (!GetWorldTimerManager().IsTimerActive(SkillSlot1TimerHandle))
	{
		FTimerDelegate Delegate;
		ActivateSkill(SkillSlot1);
		GetWorldTimerManager().SetTimer(
			SkillSlot1TimerHandle,
			Delegate,
			SkillSlot1->Cooldown,
			false
			);
	}
}

void APlayerCharacter::TrySkillSlot2()
{
	if (!SkillSlot2)
		return;

	if (!GetWorldTimerManager().IsTimerActive(SkillSlot2TimerHandle))
	{
		FTimerDelegate Delegate;
		ActivateSkill(SkillSlot2);
		GetWorldTimerManager().SetTimer(
			SkillSlot2TimerHandle,
			Delegate,
			SkillSlot2->Cooldown,
			false
			);
	}
	
}

void APlayerCharacter::TrySkillSlot3()
{
	if (!SkillSlot3)
		return;

	if (!GetWorldTimerManager().IsTimerActive(SkillSlot3TimerHandle))
	{
		FTimerDelegate Delegate;
		ActivateSkill(SkillSlot3);
		GetWorldTimerManager().SetTimer(
			SkillSlot3TimerHandle,
			Delegate,
			SkillSlot3->Cooldown,
			false
			);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::TryAttack);
	PlayerInputComponent->BindAction("SkillSlot1", IE_Pressed, this, &APlayerCharacter::TrySkillSlot1);
	PlayerInputComponent->BindAction("SkillSlot2", IE_Pressed, this, &APlayerCharacter::TrySkillSlot2);
	PlayerInputComponent->BindAction("SkillSlot3", IE_Pressed, this, &APlayerCharacter::TrySkillSlot3);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlayerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlayerCharacter::TouchStopped);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);
	
	LockOnComponent->TrySoftLockToClosestActorInInputDirection();
	
}


void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	
		Jump();
	
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::HitboxEnd_Implementation()
{
	Super::HitboxEnd_Implementation();
	LockOnComponent->LockedOnActor = nullptr;
	MotionWarping->RemoveWarpTarget(FName("AttackTarget"));
}

void APlayerCharacter::InputBufferHandle_Implementation()
{
	if (CombatComponent->bInputBuffer)
	{
		CombatComponent->bInputBuffer = false;
		SetAndStartAttack();
	}
	else
	{
		CharacterState = Idle;
	}
	
}

void APlayerCharacter::SetAndStartAttack()
{
	UAttackAsset* Attack = CombatComponent->ChooseComboAttack(EquippedWeapon->BasicAttackList);
	if (Attack)
	{
		if (GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance)
		{
			LockOnComponent->RotateOwnerToTarget();
		}
		
		StartAttack(Attack->Animation);
		
		if (OnAttack.IsBound())
		{
			OnAttack.Broadcast();
		}
	}
}

void APlayerCharacter::StartAttack(UAnimMontage* AttackAnimation)
{
	CharacterState = Attacking;

	if (IsValid(LockOnComponent->GetLockedOnActor()) && !CombatComponent->CurrentAttack->SkillHeals)
	{
		ABaseCharacter* Target = Cast<ABaseCharacter>(LockOnComponent->GetLockedOnActor());
		if (GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance)
		{
			FMotionWarpingTarget MotionWarpingTarget;
			MotionWarpingTarget.Transform.SetLocation(LockOnComponent->GetLockedOnActor()->GetActorLocation());
			MotionWarping->AddOrUpdateWarpTarget(
				FName("AttackTarget"),
				MotionWarpingTarget);
			if (Target->CharacterState == Dodging)
			{
				MotionWarping->RemoveWarpTarget("AttackTarget");
			}
		}
		
		
		if (IsCharacterInDashAttackDistance())
		{
			if (!CombatComponent->CurrentAttack->AttackCategory == Skill)
			{
				CombatComponent->CurrentAttack = EquippedWeapon->DashAttack;
				
			}
			CombatComponent->ComboCounter = 0;
			PlayAnimMontage(CombatComponent->CurrentAttack->Animation);
			return;
		}
	}

	PlayAnimMontage(AttackAnimation);
}

void APlayerCharacter::EndAttack_Implementation()
{
	Super::EndAttack_Implementation();
	MotionWarping->RemoveWarpTarget(FName("AttackTarget"));
}


bool APlayerCharacter::IsCharacterInDashAttackDistance()
{
	return GetDistanceTo(LockOnComponent->GetLockedOnActor()) > CombatComponent->MinDashAttackDistance &&
		GetDistanceTo(LockOnComponent->GetLockedOnActor()) < CombatComponent->DashAttackDistance;
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
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

void APlayerCharacter::MoveRight(float Value)
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

void APlayerCharacter::Jump()
{
	if (CharacterState == Idle)
	{
		Super::Jump();
	}
}
