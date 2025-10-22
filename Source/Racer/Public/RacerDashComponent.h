#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RacerDashComponent.generated.h"

class ARacerCharacter;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class RACER_API URacerDashComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URacerDashComponent();

    UFUNCTION(BlueprintCallable, Category="Dash")
    bool TryDash();

    UFUNCTION(BlueprintPure, Category="Dash")
    bool IsDashing() const { return bIsDashing; }

    UFUNCTION(BlueprintPure, Category="Dash")
    bool IsDashReady() const { return bDashReady; }

protected:
    virtual void BeginPlay() override;

private:
    void StartDash();
    void EndDash();
    void ResetCooldown();

private:
    UPROPERTY()
    ARacerCharacter* OwnerCharacter;

    UPROPERTY()
    UCharacterMovementComponent* OwnerMovement;

    UPROPERTY(EditAnywhere, Category="Dash")
    float DashSpeedMultiplier = 2.25f;

    UPROPERTY(EditAnywhere, Category="Dash")
    float DashDurationSeconds = 0.35f;

    UPROPERTY(EditAnywhere, Category="Dash")
    float DashCooldownSeconds = 0.60f;

    UPROPERTY(EditAnywhere, Category="Dash")
    float DashLaunchStrength = 1200.0f; // horizontal impulse

    UPROPERTY(EditAnywhere, Category="Dash")
    bool bLaunchOnlyWhenGrounded = true;

    UPROPERTY(VisibleInstanceOnly, Category="Dash")
    bool bIsDashing = false;

    UPROPERTY(VisibleInstanceOnly, Category="Dash")
    bool bDashReady = true;

    float OriginalMaxWalkSpeed = 600.0f;
    float OriginalGroundFriction = 8.0f;
    float OriginalBrakingFrictionFactor = 2.0f;

    FTimerHandle DashTimerHandle;
    FTimerHandle CooldownTimerHandle;
};
