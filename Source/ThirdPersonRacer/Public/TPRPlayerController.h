#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPRPlayerController.generated.h"

UCLASS()
class THIRDPERSONRACER_API ATPRPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void SetupInputComponent() override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookYaw(float Value);
    void LookPitch(float Value);
    void StartSprint();
    void StopSprint();
    void Dash();
};
