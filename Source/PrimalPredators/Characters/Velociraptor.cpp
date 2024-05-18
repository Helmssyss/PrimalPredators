#include "Velociraptor.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "PrimalPredators/GameModes/GM_Play.h"
#include "PrimalPredators/Other/RaptorFootprintL.h"
#include "PrimalPredators/Other/RaptorFootprintR.h"
#include "PrimalPredators/PlayerControllers/PC_Play.h"

AVelociraptor::AVelociraptor(){
	const static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClass(TEXT("/Game/Dinosaurs/Velociraptor/Animations/BPA_Raptor"));
	const static ConstructorHelpers::FObjectFinder<USkeletalMesh> RaptorMesh(TEXT("/Game/Dinosaurs/Velociraptor/Mesh/Raptor_SM"));

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(100.f);
	GetCapsuleComponent()->SetCapsuleRadius(39.f);
	GetCapsuleComponent()->bHiddenInGame = false;
	
	GetMesh()->SetAnimClass(AnimClass.Object->GeneratedClass);
	GetMesh()->SetSkeletalMesh(RaptorMesh.Object);
	GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	CameraBoom->SetupAttachment(GetMesh()); // GetMesh(),FName("Cnt_Head_JNT_SKL")
	CameraBoom->SetRelativeLocation(FVector(-6.504364, 129.960068, 308.225250));
	CameraBoom->SocketOffset = FVector(0.0, 0.0, 56.0);

	Camera->SetRelativeScale3D(FVector(1,1,1));
	Camera->SetRelativeRotation(FRotator(-10.0, 0.0, 0.0));
	
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.f, 0.0));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	
	BodyHitSphere->SetupAttachment(GetMesh(), FName("Cnt_Spine_005_JNT_SKL"));
	BodyHitSphere->SetRelativeLocation(FVector(-7.584190, 0.858286, 6.818002));
	BodyHitSphere->SetRelativeRotation(FRotator(-0.729329, 89.591377, -164.383224));
	BodyHitSphere->SetSphereRadius(50.422802);
	BodyHitSphere->bHiddenInGame = false;

	DecalFootRight->SetupAttachment(GetMesh(),FName("Rht_Back_AIk004_JNT_SKL"));
	DecalFootRight->SetRelativeLocation(FVector(8.553019,1.291424,-0.558208));
	DecalFootRight->SetRelativeRotation(FRotator(30.000013,0.0,0.0));
	DecalFootRight->SetRelativeScale3D(FVector(0.252638,0.168815,1.0));
	
	DecalFootLeft->SetupAttachment(GetMesh(),FName("Lft_Back_AIk004_JNT_SKL"));
	DecalFootLeft->SetRelativeLocation(FVector(8.735242,-1.020378,-0.941731));
	DecalFootLeft->SetRelativeRotation(FRotator(30.000013,0.0,0.0));
	DecalFootLeft->SetRelativeScale3D(FVector(0.252638,0.168815,1.0));
	
	PredatorRunSpeed = 1300.f;
	PredatorWalkSpeed = 650.f;
	MaxHealth = RAPTOR_MAX_HEALTH;
	CurrentHealth = MaxHealth;
	GetCharacterMovement()->MaxWalkSpeed = PredatorWalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 520.f;
	SetDeadState(false);
}

UClass* AVelociraptor::GetRightFootprintClass() const{
	return ARaptorFootprintR::StaticClass();
}

UClass* AVelociraptor::GetLeftFootprintClass() const{
	return ARaptorFootprintL::StaticClass();
}

void AVelociraptor::BeginPlay(){
	Super::BeginPlay();
	MulticastSlopTimer();
}

void AVelociraptor::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);
}

bool AVelociraptor::ServerPredatorRun_Validate(const float Speed){
	return Speed >= PredatorWalkSpeed && Speed <= PredatorRunSpeed;
}

void AVelociraptor::TurnRl(const float Rate) {
	Super::TurnRl(Rate);
	SmoothRotation();
}

void AVelociraptor::Jump() {
	if (!GetDeadState())
		Super::Jump();
}

void AVelociraptor::MoveFb(float Value){
	if (!GetDeadState())
		Super::MoveFb(Value);
}

void AVelociraptor::CameraLengthUp(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, 400.f, 600.f) - 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

void AVelociraptor::CameraLengthDown(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, 400.f, 600.f) + 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

EPredatorType AVelociraptor::GetPredatorType() const{
	return EPredatorType::Velociraptor;
}

void AVelociraptor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AVelociraptor::Jump);
}

UAnimMontage* AVelociraptor::GetPredatorRoarAnimMontage(){
	return LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Dinosaurs/Velociraptor/Animations/AM_ROAR"));
}

UAnimMontage* AVelociraptor::GetPredatorBiteAnimMontage() {
	return LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Dinosaurs/Velociraptor/Animations/AM_BITE"));
}

void AVelociraptor::ServerSmoothSetActorRotation_Implementation(const FRotator Rotation){
	Super::ServerSmoothSetActorRotation_Implementation(Rotation);
	float Arc = bIsRunning ? 50.f * 2.f : 50.f;
	AddMovementInput(GetActorForwardVector() * Arc);
}

void AVelociraptor::ClientSmoothSetActorRotation_Implementation(const FRotator Rotation){
	Super::ClientSmoothSetActorRotation_Implementation(Rotation);
	float Arc = bIsRunning ? 50.f * 2.f : 50.f;
	AddMovementInput(GetActorForwardVector() * Arc);
}

float AVelociraptor::GetDirectRotationInterp() const{
	return 0.3f;
}

void AVelociraptor::MulticastPlayBiteMontage_Implementation() {
	if (!bIsBite) {
		PlayBiteMontage();
		bIsBite = true;
	}
}

void AVelociraptor::MulticastPlayRoarMontage_Implementation() {
	if (!bIsRoar) {
		PlayRoarMontage();
		bIsRoar = true;
	}
}

void AVelociraptor::PlayBiteMontage() {
	if (UAnimMontage* BiteMontage = GetPredatorBiteAnimMontage()) {
		GetMesh()->GetAnimInstance()->Montage_Play(BiteMontage);
		GetWorldTimerManager().SetTimer(T_BiteHandle, this, &AVelociraptor::PlayBiteMontageEnd, 0.5f, false);
	}
}

void AVelociraptor::PlayBiteMontageEnd() {
	bIsBite = false;
	if (HasAuthority())
		BiteTrace(170, FColor::Emerald);
}

void AVelociraptor::PlayRoarMontage() {
	if (UAnimMontage* RoarMontage = GetPredatorRoarAnimMontage()) {
		GetMesh()->GetAnimInstance()->Montage_Play(RoarMontage);
		GetWorldTimerManager().SetTimer(T_RoarHandle, this, &AVelociraptor::PlayRoarMontageEnd, 1.3, false);
	}
}

void AVelociraptor::PlayRoarMontageEnd() {
	bIsRoar = false;
}

UClass* AVelociraptor::GetPredatorClass(){
	return this->StaticClass();
}

void AVelociraptor::SmoothRotation() {
	if(!bIsDead){
		DirectionTrace(FVector(Camera->GetForwardVector().X, Camera->GetForwardVector().Y, 0.f), 2000, FColor::Red);
		DirectionTrace(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f), 1000, FColor::Orange);

		const float MoveFB = GetInputAxisValue("MoveFB");
		const float MoveRL = GetInputAxisValue("MoveRL");
		const FRotator TargetRotation = DirectionModifier(MOVE_Walking);
		if(!PredatorHasGrabbed())
			if (MoveFB > 0.f || MoveFB < 0.f || MoveRL > 0.f || MoveRL < 0.f) {
				const FRotator DirectionRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, 0.107, GetDirectRotationInterp());
				if (HasAuthority())
					ClientSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
				else {
					ClientSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
					ServerSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
				}
			}
	}
}

void AVelociraptor::MulticastSlopTimer_Implementation() {
	GetWorldTimerManager().SetTimer(T_AdaptingSlopeHandle, this, &AVelociraptor::SlopAngle, 0.107, true);
}

void AVelociraptor::SlopAngle() {
	CreateLineTraceForSlope(SlopeAngleHitForwardResult, 600);
	CreateLineTraceForSlope(SlopeAngleHitBackwardResult, -600);
	const FVector GroundForwardNormal = (SlopeAngleHitForwardResult.Location - SlopeAngleHitBackwardResult.Location).GetSafeNormal();
	GroundSlopeForward = MakeRotFromX(GroundForwardNormal);
	GroundSlopeForward.Pitch = FMath::Clamp(GroundSlopeForward.Pitch, -50.f, 50.f);
}

void AVelociraptor::MulticastBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) {
	TArray<FHitResult> ResultHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->SweepMultiByChannel(ResultHit, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(50), QueryParams);
	// DrawDebugLine(GetWorld(), StartLocation, EndLocation, Color, false, 5, 0, 5);
	for (FHitResult& Hit : ResultHit) {
		if (ABasePredator* HitActor = Cast<ABasePredator>(Hit.GetActor())) {
			ImpactBloodParticle(Hit,HitActor);
			BiteDamage(HitActor);
			// DrawDebugSphere(GetWorld(), Hit.ImpactPoint, FCollisionShape::MakeSphere(50).GetSphereRadius(), 10, Color, false, 10, 0, 5);
			break;
		}
	}
}

void AVelociraptor::BiteDamage(ABasePredator* Predator) {
	const float mMaxHealth = Predator->GetMaxHealth();
	float cCurrentHealth = Predator->GetCurrentHealth();
	if (cCurrentHealth > 0) {
		cCurrentHealth -= 50.f;
		Predator->SetCurrentHealth(cCurrentHealth);
		Predator->OnHealthDelegate.Broadcast(cCurrentHealth, mMaxHealth);
	}
	if (cCurrentHealth == 0) {
		PRINT(this,FColor::Cyan,"DEAD");
		Predator->SetDeadState(true);
		Predator->GetMesh()->SetSimulatePhysics(true);
		Predator->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetIgnoreMoveInput(Predator);
		T_RespawnPredatorDelegate.BindUFunction(this,FName("Respawn"),Predator); // the "Respawn" method call in ABasePredator class.
		GetWorldTimerManager().SetTimer(T_RespawnPredatorHandle,T_RespawnPredatorDelegate,5.f,false);
	}
	OnCrosshairDamageDelegate.Broadcast(true);
}

bool AVelociraptor::ServerPredatorDamageHealthChecker_Validate(){
	if(MaxHealth == RAPTOR_MAX_HEALTH || CurrentHealth <= RAPTOR_MAX_HEALTH && CurrentHealth >= 0)
		return true;
	else{
		return false;
	}
}