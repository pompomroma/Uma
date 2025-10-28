#include "RacerGameMode.h"
#include "RacerCharacter.h"
#include "RacerPlayerController.h"

ARacerGameMode::ARacerGameMode()
{
    DefaultPawnClass = ARacerCharacter::StaticClass();
    PlayerControllerClass = ARacerPlayerController::StaticClass();
}
