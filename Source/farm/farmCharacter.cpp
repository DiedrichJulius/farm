// Copyright Epic Games, Inc. All Rights Reserved.

#include "farmCharacter.h"
#include "farmInteractableActor.h" 

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h" // for GEngine debug messages


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AfarmCharacter

AfarmCharacter::AfarmCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	CurrentlyFocusedActor = nullptr;
}

void AfarmCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AfarmCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AfarmCharacter::DoJumpEnd);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AfarmCharacter::MoveInput);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AfarmCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AfarmCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}
}

void AfarmCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TraceForInteractables();
}

void AfarmCharacter::TraceForInteractables()
{
	if (!FirstPersonCameraComponent) return;

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * 500.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	AfarmInteractableActor* HitActor = Cast<AfarmInteractableActor>(Hit.GetActor());

	if (HitActor != nullptr)
	{
		if (HitActor != CurrentlyFocusedActor)
		{
			if (CurrentlyFocusedActor)
			{
				CurrentlyFocusedActor->OnUnfocused();

				// 🔴 Hide old prompt
				if (InteractPromptInstance)
				{
					InteractPromptInstance->RemoveFromParent();
					InteractPromptInstance = nullptr;
				}
			}

			HitActor->OnFocused();
			CurrentlyFocusedActor = HitActor;

			// 🟢 Show new prompt
			if (!InteractPromptInstance && InteractPromptClass)
			{
				InteractPromptInstance = CreateWidget<UUserWidget>(GetWorld(), InteractPromptClass);
				if (InteractPromptInstance)
				{
					InteractPromptInstance->AddToViewport();
				}
			}
		}
	}
	else
	{
		// Player is not looking at any interactable
		if (CurrentlyFocusedActor)
		{
			CurrentlyFocusedActor->OnUnfocused();
			CurrentlyFocusedActor = nullptr;

			// 🔴 Remove the prompt from screen
			if (InteractPromptInstance)
			{
				InteractPromptInstance->RemoveFromParent();
				InteractPromptInstance = nullptr;
			}
		}
	}
}



// ---- Remaining movement/aim logic ----

void AfarmCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AfarmCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AfarmCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AfarmCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AfarmCharacter::DoJumpStart()
{
	Jump();
}

void AfarmCharacter::DoJumpEnd()
{
	StopJumping();
}
