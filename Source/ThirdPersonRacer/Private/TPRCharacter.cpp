#include "TPRCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ATPRCharacter::ATPRCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 12.0f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    UCharacterMovementComponent* Move = GetCharacterMovement();
    Move->bOrientRotationToMovement = true;
    Move->RotationRate = FRotator(0.f, 720.f, 0.f);
    Move->MaxWalkSpeed = 600.f;
    Move->MaxWalkSpeedCrouched = 300.f;
    Move->BrakingDecelerationWalking = 2048.f;

    MaxRunSpeed = 900.f;
    Acceleration = 4096.f;
    BrakingDeceleration = 2048.f;

    DashImpulse = 2200.f;
    DashCooldown = 1.0f;
    FOVDuringDash = 95.f;
    FOVInterpSpeed = 8.0f;

    CameraLagSpeedNormal = 12.0f;
    CameraLagSpeedDash = 18.0f;

    bWantsToSprint = false;
    bIsDashing = false;
    LastDashTime = -1000.f;
}

void ATPRCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultFOV = FollowCamera->FieldOfView;
}

void ATPRCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Apply acceleration-based speed control
    UCharacterMovementComponent* Move = GetCharacterMovement();
    const float TargetSpeed = bWantsToSprint ? MaxRunSpeed : 600.f;
    const float CurrentSpeed = Move->MaxWalkSpeed;
    const float NewSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaSeconds, bWantsToSprint ? 6.0f : 8.0f);
    Move->MaxWalkSpeed = NewSpeed;

    UpdateCameraEffects(DeltaSeconds);
}

void ATPRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATPRCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATPRCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPRCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPRCharacter::StopSprint);
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATPRCharacter::Dash);
}

void ATPRCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.f)
    {
        FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void ATPRCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.f)
    {
        FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void ATPRCharacter::LookYaw(float Value)
{
    AddControllerYawInput(Value);
}

void ATPRCharacter::LookPitch(float Value)
{
    AddControllerPitchInput(Value);
}

void ATPRCharacter::StartSprint()
{
    bWantsToSprint = true;
}

void ATPRCharacter::StopSprint()
{
    bWantsToSprint = false;
}

void ATPRCharacter::Dash()
{
    const float TimeSeconds = GetWorld()->GetTimeSeconds();
    if (TimeSeconds - LastDashTime < DashCooldown || bIsDashing)
    {
        return;
    }

    FVector Forward2D = GetActorForwardVector();
    Forward2D.Z = 0.f;
    Forward2D.Normalize();

    LaunchCharacter(Forward2D * DashImpulse + FVector(0,0,50.f), true, true);

    bIsDashing = true;
    LastDashTime = TimeSeconds;
}

void ATPRCharacter::UpdateCameraEffects(float DeltaSeconds)
{
    const float TimeSeconds = GetWorld()->GetTimeSeconds();
    const bool bDashWindow = (TimeSeconds - LastDashTime) < 0.25f;

    if (bDashWindow)
    {
        FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, FOVDuringDash, DeltaSeconds, FOVInterpSpeed));
        CameraBoom->CameraLagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, CameraLagSpeedDash, DeltaSeconds, 6.0f);
    }
    else
    {
        FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, DefaultFOV, DeltaSeconds, FOVInterpSpeed));
        CameraBoom->CameraLagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, CameraLagSpeedNormal, DeltaSeconds, 6.0f);
        bIsDashing = false;
    }
}
