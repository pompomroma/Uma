#include "TPRGameMode.h"
#include "TPRCharacter.h"
#include "TPRPlayerController.h"

ATPRGameMode::ATPRGameMode()
{
    DefaultPawnClass = ATPRCharacter::StaticClass();
    PlayerControllerClass = ATPRPlayerController::StaticClass();
}
