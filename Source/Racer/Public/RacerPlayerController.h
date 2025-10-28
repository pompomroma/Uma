#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RacerPlayerController.generated.h"

UCLASS()
class RACER_API ARacerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARacerPlayerController();

protected:
    virtual void SetupInputComponent() override;

    // Touch handling for iOS: double-tap anywhere to dash
    void OnTouchBegin(ETouchIndex::Type FingerIndex, FVector Location);
    void OnTouchEnd(ETouchIndex::Type FingerIndex, FVector Location);

private:
    float LastTapTimeSeconds = -1000.0f;
    FVector2D LastTapScreenPos = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, Category="Dash")
    float TouchDoubleTapMaxInterval = 0.30f;

    UPROPERTY(EditAnywhere, Category="Dash")
    float TouchDoubleTapMaxDistance = 40.0f; // in screen units
};
