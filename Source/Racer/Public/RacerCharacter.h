#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RacerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class URacerDashComponent;

UCLASS()
class RACER_API ARacerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARacerCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void OnDashStarted();
    void OnDashEnded();

    UFUNCTION(BlueprintCallable, Category="Dash")
    void RequestDash();

protected:
    virtual void BeginPlay() override;

private:
    // Input handlers
    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartJump();
    void StopJump();
    void DashAction();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dash", meta=(AllowPrivateAccess="true"))
    URacerDashComponent* DashComponent;

    // Camera tuning
    UPROPERTY(EditAnywhere, Category="Camera")
    float DefaultFOV = 90.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float DashFOV = 100.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float FOVInterpSpeed = 10.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float DefaultArmLength = 500.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float DashArmLength = 470.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float ArmInterpSpeed = 8.0f;

    // Movement tuning
    UPROPERTY(EditAnywhere, Category="Movement")
    float BaseWalkSpeed = 650.0f;

    // Double-tap detection for forward dash
    float LastForwardValue = 0.0f;
    float LastForwardPressTime = -1000.0f;

    UPROPERTY(EditAnywhere, Category="Dash")
    float ForwardDoubleTapMaxInterval = 0.33f;

    // Runtime targets for smooth camera feedback
    float TargetFOV = 90.0f;
    float TargetArmLength = 500.0f;
};
