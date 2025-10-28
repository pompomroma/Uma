#include "RacerPlayerController.h"
#include "RacerCharacter.h"
#include "Engine/World.h"

ARacerPlayerController::ARacerPlayerController()
{
    bEnableTouchEvents = true;
    bShowMouseCursor = false;
}

void ARacerPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindTouch(IE_Pressed, this, &ARacerPlayerController::OnTouchBegin);
        InputComponent->BindTouch(IE_Released, this, &ARacerPlayerController::OnTouchEnd);
    }
}

void ARacerPlayerController::OnTouchBegin(ETouchIndex::Type FingerIndex, FVector Location)
{
    // Intentionally empty; we only need end events for stable timing
}

void ARacerPlayerController::OnTouchEnd(ETouchIndex::Type FingerIndex, FVector Location)
{
    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    FVector2D ScreenPos;
    ProjectWorldLocationToScreen(Location, ScreenPos);

    bool bIsDoubleTap = false;
    const float TimeSinceLast = Now - LastTapTimeSeconds;
    if (TimeSinceLast <= TouchDoubleTapMaxInterval)
    {
        const float Distance = FVector2D::Distance(ScreenPos, LastTapScreenPos);
        if (Distance <= TouchDoubleTapMaxDistance)
        {
            bIsDoubleTap = true;
        }
    }

    LastTapTimeSeconds = Now;
    LastTapScreenPos = ScreenPos;

    if (bIsDoubleTap)
    {
        if (APawn* P = GetPawn())
        {
            if (ARacerCharacter* RC = Cast<ARacerCharacter>(P))
            {
                RC->RequestDash();
            }
        }
    }
}
