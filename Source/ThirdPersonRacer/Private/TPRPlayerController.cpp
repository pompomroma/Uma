#include "TPRPlayerController.h"

void ATPRPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // Input is bound in ATPRCharacter::SetupPlayerInputComponent
}

void ATPRPlayerController::MoveForward(float) {}
void ATPRPlayerController::MoveRight(float) {}
void ATPRPlayerController::LookYaw(float) {}
void ATPRPlayerController::LookPitch(float) {}
void ATPRPlayerController::StartSprint() {}
void ATPRPlayerController::StopSprint() {}
void ATPRPlayerController::Dash() {}
