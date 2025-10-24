#include "RacerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "RacerDashComponent.h"

ARacerCharacter::ARacerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Movement
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true; // Rotate towards movement direction
    MoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    MoveComp->MaxWalkSpeed = BaseWalkSpeed;
    MoveComp->BrakingDecelerationWalking = 2048.0f;
    MoveComp->GroundFriction = 8.0f;
    MoveComp->AirControl = 0.35f;

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // Camera boom
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = DefaultArmLength;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 12.0f;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;
    SpringArm->bInheritYaw = true;
    SpringArm->SocketOffset = FVector(0.f, 60.f, 60.f);

    // Follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false; // Camera rotates via spring arm
    FollowCamera->FieldOfView = DefaultFOV;
    FollowCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
    FollowCamera->PostProcessSettings.MotionBlurAmount = 0.5f; // Tunable for speed feel

    // Dash component
    DashComponent = CreateDefaultSubobject<URacerDashComponent>(TEXT("DashComponent"));

    TargetFOV = DefaultFOV;
    TargetArmLength = DefaultArmLength;
}

void ARacerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Ensure movement speed is aligned with editable BaseWalkSpeed
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = BaseWalkSpeed;
    }

    // Initialize camera targets
    TargetFOV = DefaultFOV;
    TargetArmLength = DefaultArmLength;
}

void ARacerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Smooth FOV and boom length adjustments
    if (FollowCamera)
    {
        const float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, FOVInterpSpeed);
        FollowCamera->SetFieldOfView(NewFOV);
    }
    if (SpringArm)
    {
        const float NewArm = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, ArmInterpSpeed);
        SpringArm->TargetArmLength = NewArm;
    }
}

void ARacerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARacerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ARacerCharacter::MoveRight);

    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ARacerCharacter::StartJump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ARacerCharacter::StopJump);

    PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ARacerCharacter::DashAction);
}

void ARacerCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        // Find out which way is forward relative to controller yaw
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }

    // Rising-edge detection for double-tap dash when moving forward
    const float Now = GetWorld()->GetTimeSeconds();
    const bool bRising = (Value > 0.6f && LastForwardValue <= 0.6f);
    if (bRising)
    {
        if ((Now - LastForwardPressTime) <= ForwardDoubleTapMaxInterval)
        {
            RequestDash();
        }
        LastForwardPressTime = Now;
    }
    LastForwardValue = Value;
}

void ARacerCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void ARacerCharacter::StartJump()
{
    Jump();
}

void ARacerCharacter::StopJump()
{
    StopJumping();
}

void ARacerCharacter::DashAction()
{
    RequestDash();
}

void ARacerCharacter::RequestDash()
{
    if (DashComponent)
    {
        DashComponent->TryDash();
    }
}

void ARacerCharacter::OnDashStarted()
{
    TargetFOV = DashFOV;
    TargetArmLength = DashArmLength;
}

void ARacerCharacter::OnDashEnded()
{
    TargetFOV = DefaultFOV;
    TargetArmLength = DefaultArmLength;
}
