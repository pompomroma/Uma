#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPRCharacter.generated.h"

UCLASS()
class THIRDPERSONRACER_API ATPRCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATPRCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StartSprint();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void StopSprint();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Dash();

protected:
    // Camera boom positioning the camera behind the character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* CameraBoom;

    // Follow camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* FollowCamera;

    // Movement parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    float MaxRunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    float Acceleration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    float BrakingDeceleration;

    // Dash parameters
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
    float DashImpulse;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
    float DashCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
    float FOVDuringDash;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
    float FOVInterpSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
    float CameraLagSpeedNormal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
    float CameraLagSpeedDash;

private:
    bool bWantsToSprint;
    bool bIsDashing;
    float LastDashTime;
    float DefaultFOV;

    FVector MovementInput;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookYaw(float Value);
    void LookPitch(float Value);

    void UpdateCameraEffects(float DeltaSeconds);
};
