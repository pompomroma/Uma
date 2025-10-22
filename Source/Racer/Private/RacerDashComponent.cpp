#include "RacerDashComponent.h"
#include "RacerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

URacerDashComponent::URacerDashComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URacerDashComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ARacerCharacter>(GetOwner());
    if (OwnerCharacter)
    {
        OwnerMovement = OwnerCharacter->GetCharacterMovement();
        if (OwnerMovement)
        {
            OriginalMaxWalkSpeed = OwnerMovement->MaxWalkSpeed;
            OriginalGroundFriction = OwnerMovement->GroundFriction;
            OriginalBrakingFrictionFactor = OwnerMovement->BrakingFrictionFactor;
        }
    }
}

bool URacerDashComponent::TryDash()
{
    if (!OwnerCharacter || !OwnerMovement)
    {
        return false;
    }

    if (bIsDashing || !bDashReady)
    {
        return false;
    }

    StartDash();
    return true;
}

void URacerDashComponent::StartDash()
{
    bIsDashing = true;
    bDashReady = false;

    // Speed boost and lowered friction for snappy dash
    OwnerMovement->MaxWalkSpeed = OriginalMaxWalkSpeed * DashSpeedMultiplier;
    OwnerMovement->GroundFriction = 0.0f;
    OwnerMovement->BrakingFrictionFactor = 0.0f;

    // Horizontal impulse in facing direction
    if (!bLaunchOnlyWhenGrounded || OwnerMovement->IsMovingOnGround())
    {
        const FVector Forward = OwnerCharacter->GetActorForwardVector();
        const FVector LaunchVelocity = Forward * DashLaunchStrength;
        OwnerCharacter->LaunchCharacter(LaunchVelocity, true, false);
    }

    // Notify character for camera/FOV feedback
    OwnerCharacter->OnDashStarted();

    // End dash after duration, then start cooldown
    GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &URacerDashComponent::EndDash, DashDurationSeconds, false);
}

void URacerDashComponent::EndDash()
{
    if (!OwnerMovement)
    {
        return;
    }

    // Restore movement parameters
    OwnerMovement->MaxWalkSpeed = OriginalMaxWalkSpeed;
    OwnerMovement->GroundFriction = OriginalGroundFriction;
    OwnerMovement->BrakingFrictionFactor = OriginalBrakingFrictionFactor;

    bIsDashing = false;

    if (OwnerCharacter)
    {
        OwnerCharacter->OnDashEnded();
    }

    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &URacerDashComponent::ResetCooldown, DashCooldownSeconds, false);
}

void URacerDashComponent::ResetCooldown()
{
    bDashReady = true;
}
