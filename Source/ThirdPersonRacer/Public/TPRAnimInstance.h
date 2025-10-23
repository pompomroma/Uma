#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPRAnimInstance.generated.h"

UCLASS()
class THIRDPERSONRACER_API UTPRAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInAir = false;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
