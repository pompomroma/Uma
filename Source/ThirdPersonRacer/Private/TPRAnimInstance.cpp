#include "TPRAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/Character.h"

void UTPRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ACharacter* OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (!OwnerCharacter)
    {
        Speed = 0.f;
        bIsInAir = false;
        return;
    }

    FVector Velocity = OwnerCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = OwnerCharacter->GetMovementComponent()->IsFalling();
}
