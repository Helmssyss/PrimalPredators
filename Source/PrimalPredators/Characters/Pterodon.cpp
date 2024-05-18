#include "Pterodon.h"
#include "PrimalPredators/Other/PterodonFootprintL.h"
#include "PrimalPredators/Other/PterodonFootprintR.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

APterodon::APterodon(){
	PrimaryActorTick.bCanEverTick = false;
	const static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClass(TEXT("/Game/Dinosaurs/Pterodon/Animations/BPA_Pterodon"));
	const static ConstructorHelpers::FObjectFinder<USkeletalMesh> PteraMesh(TEXT("/Game/Dinosaurs/Pterodon/Mesh/Ptero"));
	
	GetMesh()->SetAnimClass(AnimClass.Object->GeneratedClass);
	GetMesh()->SetSkeletalMesh(PteraMesh.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -18.681961));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	Camera->SetRelativeRotation(FRotator(-10.0,0.0,0.0));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetRelativeLocation(FVector(0.0,0.0,284.610321));
	CameraBoom->TargetArmLength = 545.085876;
	CameraBoom->SocketOffset = FVector(0.0,0.0,152.157623);

	BodyHitSphere->SetupAttachment(GetMesh(), FName("c_back4"));
	BodyHitSphere->SetSphereRadius(55.0);
	BodyHitSphere->bHiddenInGame = false;

	DecalFootRight->SetupAttachment(GetMesh(), FName("r_ankle"));
	DecalFootLeft->SetupAttachment(GetMesh(), FName("l_ankle"));
	
	PredatorRunSpeed = 190.f;
	PredatorWalkSpeed = 100.f;
	PredatorMaxFlySpeed = 6700.f;
	PredatorDefaultFlySpeed = 1000.f;
	PredatorGlideSpeed = 600.f;

	GetCharacterMovement()->MaxFlySpeed = PredatorDefaultFlySpeed;
	GetCharacterMovement()->MaxWalkSpeed = PredatorWalkSpeed;
	GetCapsuleComponent()->SetCapsuleHalfHeight(132);
	GetCapsuleComponent()->SetCapsuleRadius(132);
	GetCapsuleComponent()->bHiddenInGame = false;
	GetCapsuleComponent()->SetIsReplicated(true);
}

UClass* APterodon::GetPredatorClass(){
	return this->StaticClass();
}

UClass* APterodon::GetRightFootprintClass() const{
	return APterodonFootprintR::StaticClass();
}

UClass* APterodon::GetLeftFootprintClass() const{
	return APterodonFootprintL::StaticClass();
}

void APterodon::BeginPlay(){
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APterodon::CapsuleHitLand);
	MulticastSlopTimer();
}

void APterodon::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);
}

void APterodon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&APterodon::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released,this,&APterodon::ServerFlyingUpFinished);
	PlayerInputComponent->BindAction("FlyingDown",IE_Pressed,this,&APterodon::ServerFlyingDown);
	PlayerInputComponent->BindAction("FlyingDown",IE_Released,this,&APterodon::ServerFlyingDownFinished);
	PlayerInputComponent->BindAction("LookAroundAir", IE_Pressed, this, &APterodon::LookAroundAirPressed);
	PlayerInputComponent->BindAction("LookAroundAir", IE_Released, this, &APterodon::LookAroundAirReleased);
	PlayerInputComponent->BindAction("AirBrake", IE_Pressed, this, &APterodon::AirBrake);
	PlayerInputComponent->BindAction("AirBrake", IE_Released, this, &APterodon::AirBrakeReleased);
}

void APterodon::TurnRl(const float Rate) {
	Super::TurnRl(Rate);
	SmoothRotation();
}

bool APterodon::ServerPredatorRun_Validate(const float Speed){
	return Speed >= PredatorWalkSpeed && Speed <= PredatorRunSpeed;
}

EPredatorType APterodon::GetPredatorType() const{
	return EPredatorType::Pterodon;
}

float APterodon::GetDirectRotationInterp() const{
	return 0.2f;
}

void APterodon::SmoothRotation(){
	if(!bIsDead){
		DirectionTrace(FVector(Camera->GetForwardVector().X, Camera->GetForwardVector().Y, 0.f), 2000, FColor::Red);
		DirectionTrace(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f), 1000, FColor::Orange);

		FRotator TargetRotation;
		const float MoveFB = GetInputAxisValue("MoveFB");
		const float MoveRL = GetInputAxisValue("MoveRL");
		
		if(GetCharacterMovement()->MovementMode == MOVE_Flying)
			TargetRotation = DirectionModifier(MOVE_Flying);
		else
			TargetRotation = DirectionModifier(MOVE_Walking);

		if (!PredatorHasGrabbed())
			if (MoveFB > 0.f || MoveFB < 0.f || MoveRL > 0.f || MoveRL < 0.f || bIsGlide || bIsBrake) {
				const FRotator DirectionRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, 0.107, GetDirectRotationInterp());
				if (HasAuthority())
					ClientSmoothSetActorRotation(bIsLookAround && GetCharacterMovement()->IsFlying() ? GetActorRotation() : FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
				else {
					ClientSmoothSetActorRotation(bIsLookAround && GetCharacterMovement()->IsFlying() ? GetActorRotation() : FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
					ServerSmoothSetActorRotation(bIsLookAround && GetCharacterMovement()->IsFlying() ? GetActorRotation() : FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
				}
			}
	}
}

void APterodon::CameraLengthUp(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, 400.f, 600.f) - 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

void APterodon::CameraLengthDown(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, 400.f, 600.f) + 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

void APterodon::MulticastSlopTimer_Implementation() {
	GetWorldTimerManager().SetTimer(T_AdaptingSlopeHandle, this, &APterodon::SlopAngle, 0.107, true);
}

void APterodon::SlopAngle() {
	if(GetCharacterMovement()->MovementMode != MOVE_Flying){
		CreateLineTraceForSlope(SlopeAngleHitForwardResult, 600);
		CreateLineTraceForSlope(SlopeAngleHitBackwardResult, -600);
		const FVector GroundForwardNormal = (SlopeAngleHitForwardResult.Location - SlopeAngleHitBackwardResult.Location).GetSafeNormal();
		GroundSlopeForward = MakeRotFromX(GroundForwardNormal);
		GroundSlopeForward.Pitch = FMath::Clamp(GroundSlopeForward.Pitch, -50.f, 50.f);	
	}
}

void APterodon::ServerSmoothSetActorRotation_Implementation(FRotator Rotation){
	Super::ServerSmoothSetActorRotation_Implementation(Rotation);
	float Arc;
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
		Arc = 720.f;
	else{
		if (bIsRunning)
			Arc = 1100.f * 2.f;
		else
			Arc = 1100.f;
	}
	AddMovementInput(GetActorForwardVector() * Arc);
}

void APterodon::ClientSmoothSetActorRotation_Implementation( FRotator Rotation){
	Super::ClientSmoothSetActorRotation_Implementation(Rotation);
	float Arc;
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
		if(bIsRunning)
			Arc = 720.f * 2.f;
		else
			Arc = 720.f;
	else{
		if (bIsRunning)
			Arc = 1100.f * 2.f;
		else
			Arc = 1100.f;
	}
	AddMovementInput(GetActorForwardVector() * Arc);
}

void APterodon::Jump(){
	if(!GetDeadState() || !PredatorHasGrabbed())
		Super::Jump();

	if(GetCharacterMovement()->MovementMode != MOVE_Flying && GetCharacterMovement()->IsFalling()){
		ServerSetMovementMode(MOVE_Flying);
		ServerFlyingState(true);
	}
	
	if(GetCharacterMovement()->MovementMode == MOVE_Flying || bIsGlide){
		PRINT(this,FColor::Cyan,"HAVADA");
		ServerFlyingUp();
	}
}

void APterodon::RunStart(){
	const float MoveFB = GetInputAxisValue("MoveFB");
	Super::RunStart();
	if (GetCharacterMovement()->MovementMode == MOVE_Flying && MoveFB > 0.f)
		SpeedFlyStart();

	ServerSetGlideState(false);
}

void APterodon::RunEnd(){
	Super::RunEnd();
	SpeedFlyEnd();
}

void APterodon::MoveFb(float Value){
	Super::MoveFb(Value);
	 if(Value == 0.f && GetCharacterMovement()->IsFlying()){
	 	ServerGlide();
	 }

	 if (Value == 1.f && !bIsGlide && bIsFlying){
		 FreeAppendSpeed();
		 if(GetCharacterMovement()->Velocity.Size() <= PredatorDefaultFlySpeed)
			ServerSpeedFly(PredatorDefaultFlySpeed);

		 if(GetCharacterMovement()->Velocity.Size() >= PredatorMaxFlySpeed)
			 ServerSpeedFly(PredatorMaxFlySpeed);
	 }

	 if(Value == 1.f && GetCharacterMovement()->IsFlying()){
		 ServerSetGlideState(false);
	 }
}

void APterodon::SpeedFlyStart(){
	ServerSetFastFlyState(true);
	GetWorldTimerManager().SetTimer(T_SpeedFlyingHandle,this,&APterodon::SpeedFlyStartWithTimer,0.107,true);
}

void APterodon::SpeedFlyEnd(){
	ServerSetFastFlyState(false);
	GetWorldTimerManager().ClearTimer(T_SpeedFlyingHandle);
	GetWorldTimerManager().SetTimer(T_SpeedFlyingHandle,this,&APterodon::SpeedFlyEndWithTimer,0.107,true);
}

void APterodon::LookAroundAirPressed(){
	bIsLookAround = true;
}

void APterodon::LookAroundAirReleased(){
	bIsLookAround = false;
}

void APterodon::SpeedFlyStartWithTimer(){
	GetCharacterMovement()->MaxFlySpeed = FMath::FInterpTo(GetCharacterMovement()->MaxFlySpeed,PredatorMaxFlySpeed,GetWorld()->GetDeltaSeconds(),5);
	ServerSpeedFly(GetCharacterMovement()->MaxFlySpeed);
}

void APterodon::SpeedFlyEndWithTimer(){
	GetWorldTimerManager().ClearTimer(T_SpeedFlyingHandle);
}

bool APterodon::ServerSpeedFly_Validate(const float FlySpeed){
	return FlySpeed >= 0.f && FlySpeed <= PredatorMaxFlySpeed;
}

void APterodon::ServerSpeedFly_Implementation(const float FlySpeed){
	GetCharacterMovement()->MaxFlySpeed = FlySpeed;
}

void APterodon::ServerSetMovementMode_Implementation(const EMovementMode NewMovementMode){
	MulticastSetMovementMode(NewMovementMode);
}

void APterodon::MulticastSetMovementMode_Implementation(const EMovementMode NewMovementMode){
	GetCharacterMovement()->BrakingDecelerationFlying = 0;//5000.f;
	GetCharacterMovement()->MaxAcceleration = 700000.f;
	GetCharacterMovement()->SetMovementMode(NewMovementMode);
}

void APterodon::ServerFlyingUp_Implementation(){
	MulticastFlyingUp();
}

void APterodon::MulticastFlyingUp_Implementation(){
	bIsFlyingUp = true;
	GetWorldTimerManager().SetTimer(T_FlyingPitchHandle,this,&APterodon::FlyingSetActorPitch,0.01,true);
	GetWorldTimerManager().SetTimer(T_FlyingUpHandle,this,&APterodon::FlyingUpStart,0.01,true);
}

void APterodon::ServerFlyingUpFinished_Implementation(){
	MulticastFlyingUpFinished();
}

void APterodon::MulticastFlyingUpFinished_Implementation(){
	if(GetCharacterMovement()->MovementMode == MOVE_Flying){
		GetWorldTimerManager().ClearTimer(T_FlyingUpHandle);
		GetWorldTimerManager().ClearTimer(T_FlyingPitchHandle);
	}
}

void APterodon::ServerFlyingDown_Implementation(){
	MulticastFlyingDown();
}

void APterodon::MulticastFlyingDown_Implementation(){
	bIsFlyingUp = false;
	GetWorldTimerManager().SetTimer(T_FlyingPitchHandle,this,&APterodon::FlyingSetActorPitch,0.01,true);
	GetWorldTimerManager().SetTimer(T_FlyingDownHandle,this,&APterodon::FlyingDownStart,0.01,true);
}

void APterodon::ServerFlyingDownFinished_Implementation(){
	MulticastFlyingDownFinished();
}

void APterodon::MulticastFlyingDownFinished_Implementation(){
	if(GetCharacterMovement()->MovementMode == MOVE_Flying){
		GetWorldTimerManager().ClearTimer(T_FlyingDownHandle);
		GetWorldTimerManager().ClearTimer(T_FlyingPitchHandle);
	}
}

void APterodon::FlyingUpStart(){
	const FVector CurrentVec = FVector(GetActorForwardVector().X,GetActorForwardVector().Y,1);
	AddMovementInput(CurrentVec);
}

void APterodon::FlyingDownStart(){
	const FVector CurrentVec = FVector(GetActorForwardVector().X,GetActorForwardVector().Y,-1);
	AddMovementInput(CurrentVec);
}

void APterodon::FlyingSetActorPitch(){
	FRotator CurrentRot = GetActorRotation();
	if(bIsFlyingUp){
		CurrentRot.Pitch = FMath::Lerp(CurrentRot.Pitch,45.f, 0.01);
		if(FMath::IsNearlyEqual(CurrentRot.Pitch,45.f,2))
			GetWorldTimerManager().ClearTimer(T_FlyingPitchHandle);
	}else{
		CurrentRot.Pitch = FMath::Lerp(CurrentRot.Pitch,-45.f, 0.01);
		if(FMath::IsNearlyEqual(CurrentRot.Pitch,-45.f,2))
			GetWorldTimerManager().ClearTimer(T_FlyingPitchHandle);
	}
	CurrentRot.Roll = FMath::Lerp(CurrentRot.Roll,0.f, 0.01);
	SetActorRotation(CurrentRot);
}

void APterodon::CapsuleHitLand(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,UPrimitiveComponent* PrimitiveComponent1, FVector Vector, const FHitResult& HitResult){
	PRINT(this,FColor::Cyan,"CapsuleHitLand");
	if(GetCharacterMovement()->IsFlying() && Actor){
		ServerResetMovement();
	}
}

void APterodon::ServerGlide_Implementation() {
	MulticastGlide();
}

void APterodon::MulticastGlide_Implementation() {
	FreeAppendSpeed();
	FVector CurrentLocation = GetActorLocation();
	bIsGlide = true;
	if (!bIsBrake) {
		//CurrentLocation.Z -= 5.f;
		//SetActorLocation(CurrentLocation);
	}
}

void APterodon::ServerResetMovement_Implementation(){
	MulticastResetMovement();
}


void APterodon::MulticastResetMovement_Implementation() {
	bIsGlide = false;
	bIsFlying = false;
	GetWorldTimerManager().ClearTimer(T_FlyingPitchHandle);
	GetWorldTimerManager().ClearTimer(T_SpeedFlyingHandle);
	GetCharacterMovement()->MaxFlySpeed = PredatorDefaultFlySpeed;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PRINT(this, FColor::Cyan, "Carpma hizi = %f", GetCharacterMovement()->Velocity.Size());
	SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
}

void APterodon::FreeAppendSpeed(){
	if (!bIsBrake) {
		FlyPitchValue = GetActorRotation().Vector().Z;
		const float MappedValue = FMath::GetMappedRangeValueClamped(FVector2D(0.009f, 0.001f), FVector2D(PredatorGlideSpeed, PredatorMaxFlySpeed), FlyPitchValue);
		GetCharacterMovement()->MaxFlySpeed = FMath::FInterpTo(GetCharacterMovement()->MaxFlySpeed, MappedValue, GetWorld()->GetDeltaSeconds(), 0.1f);
		ServerSpeedFly(GetCharacterMovement()->MaxFlySpeed);
	}
}

void APterodon::AirBrake(){
	if(GetCharacterMovement()->MovementMode == MOVE_Flying)
		GetWorldTimerManager().SetTimer(T_AirBrakeHandle, this, &APterodon::ServerSlowDownInAir, 0.107, true);
}

void APterodon::AirBrakeReleased() {
	ServerSetAirBrakeState(false);
	GetWorldTimerManager().ClearTimer(T_AirBrakeHandle);
}

void APterodon::ServerSlowDownInAir_Implementation() {
	MulticastSlowDownInAir();
}

void APterodon::MulticastSlowDownInAir_Implementation() {
	ServerSetAirBrakeState(true);
	GetCharacterMovement()->MaxFlySpeed = FMath::Lerp<float>(GetCharacterMovement()->MaxFlySpeed, 0.f, 0.1f);
	ServerSpeedFly(GetCharacterMovement()->MaxFlySpeed);
	PRINT(this, FColor::Cyan, "YAVASLIYOR | Speed = %f", GetCharacterMovement()->MaxFlySpeed);
	if (FMath::IsNearlyZero(GetCharacterMovement()->MaxFlySpeed,0.1f)) {
		PRINT(this, FColor::Orange, "YAVASLADI");
		GetWorldTimerManager().ClearTimer(T_AirBrakeHandle);
	}
}

void APterodon::ServerSetGlideState_Implementation(const bool isGlide) {
	MulticastSetGlideState(isGlide);
}

void APterodon::ServerSetAirBrakeState_Implementation(const bool isBrake) {
	MulticastSetAirBrakeState(isBrake);
}

void APterodon::MulticastSetAirBrakeState_Implementation(const bool isBrake) {
	bIsBrake = isBrake;
}

void APterodon::ServerSetFastFlyState_Implementation(bool isFastFly){
	MulticastSetFastFlyState(isFastFly);
}

void APterodon::MulticastSetFastFlyState_Implementation(bool isFastFly){
	bIsFastFly = isFastFly;
}

void APterodon::ServerFlyingState_Implementation(const bool isFlying){
	MulticastFlyingState(isFlying);
}

void APterodon::MulticastFlyingState_Implementation(const bool isFlying){
	bIsFlying = isFlying;
}

void APterodon::MulticastSetGlideState_Implementation(const bool isGlide){
	bIsGlide = isGlide;
}