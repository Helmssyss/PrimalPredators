#include "T_Rex.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "PrimalPredators/Other/RexFootprint.h"

AT_Rex::AT_Rex(){
	PrimaryActorTick.bCanEverTick = true;
	const static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimClass(TEXT("/Game/Dinosaurs/T-Rex/Animations/BPA_Rex"));
	const static ConstructorHelpers::FObjectFinder<USkeletalMesh> TRexMesh(TEXT("/Game/Dinosaurs/T-Rex/Mesh/Rex"));

	GrabBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBoxComponent"));
	GrabBoxComponent->SetupAttachment(GetMesh(),FName("Cnt_Tongue_003_JNT_SKL"));
	GrabBoxComponent->SetRelativeLocation(FVector(-10.099997, -0.000000, -23.333359));
	GrabBoxComponent->SetGenerateOverlapEvents(true);
	GrabBoxComponent->SetRelativeScale3D(FVector(4.000000, 1.500000, 1.250000));
	GrabBoxComponent->bHiddenInGame = false;
	GrabBoxSetCollision();
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(245);
	GetCapsuleComponent()->SetCapsuleRadius(245.0f);
	GetCapsuleComponent()->bHiddenInGame = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	
	GetMesh()->SetAnimClass(AnimClass.Object->GeneratedClass);
	GetMesh()->SetSkeletalMesh(TRexMesh.Object);
	GetMesh()->SetRelativeScale3D(FVector(3.0f,3.0f,3.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -245.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	CameraBoom->SetRelativeLocation(FVector(610.0, 0.0, 927.0));
	CameraBoom->SetupAttachment(GetRootComponent()); // GetMesh(),FName("Cnt_Head_JNT_SKL")
	CameraBoom->SocketOffset = FVector(0.0, 0.0, 125.0);

	BodyHitSphere->SetupAttachment(GetMesh(), FName("Cnt_Spine_002_JNT_SKL"));
	BodyHitSphere->SetRelativeLocation(FVector(-39.999981, 6.666639, 33.333321));
	BodyHitSphere->SetSphereRadius(146.29773);
	BodyHitSphere->bHiddenInGame = false;

	DecalFootRight->SetupAttachment(GetMesh(),FName("Rht_Leg_AIk004_JNT_SKL"));
	DecalFootRight->SetRelativeLocation(FVector(29.296520,0.0,-12.435639));
	DecalFootRight->SetRelativeRotation(FRotator(24.0,0.0,0.0));
	DecalFootRight->SetRelativeScale3D(FVector(1.0,0.75,1.0));
	
	DecalFootLeft->SetupAttachment(GetMesh(),FName("Lft_Leg_AIk004_JNT_SKL"));
	DecalFootLeft->SetRelativeLocation(FVector(32.843933,0.0,-9.869366));
	DecalFootLeft->SetRelativeRotation(FRotator(26.0,0.0,0.0));
	DecalFootLeft->SetRelativeScale3D(FVector(1.0,0.75,1.0));

	PredatorRunSpeed = 1200.f;
	PredatorWalkSpeed = 850.0f;
	MaxHealth = REX_MAX_HEALTH;
	CurrentHealth = MaxHealth;
	Camera->SetRelativeRotation(FRotator(-19.89, 0.0, 0.0));
	GetCharacterMovement()->MaxWalkSpeed = PredatorWalkSpeed;
	
}

UClass* AT_Rex::GetRightFootprintClass() const{
	return ARexFootprint::StaticClass();
}

UClass* AT_Rex::GetLeftFootprintClass() const{
	return ARexFootprint::StaticClass();
}

void AT_Rex::BeginPlay(){
	Super::BeginPlay();
	MulticastSlopTimer();
	GrabBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AT_Rex::GrabBoxBeginOverlap);
	GrabBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AT_Rex::GrabBoxEndOverlap);
}

void AT_Rex::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);
}

void AT_Rex::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AT_Rex, GrabPredator);
	DOREPLIFETIME(AT_Rex, bIsGrab);
	DOREPLIFETIME(AT_Rex,bIsDead);
}

void AT_Rex::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AT_Rex::PlayGrabMontage);
}

UClass* AT_Rex::GetPredatorClass(){
	return this->StaticClass();
}

void AT_Rex::TurnRl(const float Rate) {
	Super::TurnRl(Rate);
	SmoothRotation();
}

void AT_Rex::CameraLengthUp(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength,400.f,1200.f) - 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

void AT_Rex::CameraLengthDown(){
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength,400.f,1200.f) + 100;
	OnCrosshairOpacityDelegate.Broadcast(GetMaxCameraBoomLength());
}

void AT_Rex::SlopAngle(){
	CreateLineTraceForSlope(SlopeAngleHitForwardResult, 600);
	CreateLineTraceForSlope(SlopeAngleHitBackwardResult, -600);
	const FVector GroundForwardNormal = (SlopeAngleHitForwardResult.Location - SlopeAngleHitBackwardResult.Location).GetSafeNormal();
	GroundSlopeForward = MakeRotFromX(GroundForwardNormal);
}

EPredatorType AT_Rex::GetPredatorType() const{
	return EPredatorType::T_REX;
}

UAnimMontage* AT_Rex::GetPredatorRoarAnimMontage(){
	return LoadObject<UAnimMontage>(nullptr,TEXT("/Game/Dinosaurs/T-Rex/Animations/AM_Rex_ROAR"));
}

UAnimMontage* AT_Rex::GetPredatorBiteAnimMontage(){
	return LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Dinosaurs/T-Rex/Animations/AM_Rex_BITE"));
}

UAnimMontage* AT_Rex::GetPredatorGrabAnimMontage(){
	return LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Dinosaurs/T-Rex/Animations/AM_Rex_GRAB"));
}

float AT_Rex::GetDirectRotationInterp() const{
	return 0.08f;
}

void AT_Rex::PlayBiteMontage(){
	if (UAnimMontage* BiteMontage = GetPredatorBiteAnimMontage()) {
		GetMesh()->GetAnimInstance()->Montage_Play(BiteMontage);
		GetWorldTimerManager().SetTimer(T_BiteHandle, this, &AT_Rex::PlayBiteMontageEnd, BITE_MONTAGE_END, false);
	}
}

void AT_Rex::PlayBiteMontageEnd() {
	bIsBite = false;
	if(HasAuthority())
		BiteTrace(1000, FColor::Emerald);
}

void AT_Rex::PlayRoarMontage(){
	PRINT(this,FColor::Cyan,"bIsGrab = %i",bIsGrab);
	if (UAnimMontage* RoarMontage = GetPredatorRoarAnimMontage()) {
		GetMesh()->GetAnimInstance()->Montage_Play(RoarMontage);
		GetWorldTimerManager().SetTimer(T_RoarHandle, this, &AT_Rex::PlayRoarMontageEnd, ROAR_MONTAGE_END, false);
	}
}

void AT_Rex::PlayRoarMontageEnd() {
	bIsRoar = false;
}

void AT_Rex::PlayGrabMontage(){
	if(bIsGrabActorOverlap)
		ServerPlayGrabMontage(GrabPredator);
}

void AT_Rex::GrabBoxSetCollision(){
	FCollisionResponseContainer GrabBoxResponseContainer;
	GrabBoxResponseContainer.SetResponse(ECC_Visibility, ECR_Ignore);
	GrabBoxResponseContainer.SetResponse(ECC_Camera, ECR_Overlap);
	GrabBoxResponseContainer.SetResponse(ECC_WorldStatic, ECR_Overlap);
	GrabBoxResponseContainer.SetResponse(ECC_WorldDynamic, ECR_Overlap);
	GrabBoxResponseContainer.SetResponse(ECC_Pawn, ECR_Ignore);
	GrabBoxResponseContainer.SetResponse(ECC_PhysicsBody, ECR_Overlap);
	GrabBoxResponseContainer.SetResponse(ECC_Vehicle, ECR_Ignore);
	GrabBoxResponseContainer.SetResponse(ECC_Destructible, ECR_Ignore);

	GrabBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GrabBoxComponent->SetCollisionResponseToChannels(GrabBoxResponseContainer);
	GrabBoxComponent->SetCollisionProfileName("Custom");
}

void AT_Rex::SmoothRotation() {
	DirectionTrace(FVector(Camera->GetForwardVector().X, Camera->GetForwardVector().Y, 0.f), 2000, FColor::Red);
	DirectionTrace(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f), 1000, FColor::Orange);

	const float MoveFB = GetInputAxisValue("MoveFB");
	const float MoveRL = GetInputAxisValue("MoveRL");
	const FRotator TargetRotation = DirectionModifier(MOVE_Walking);

	if (MoveFB > 0.f || MoveFB < 0.f || MoveRL > 0.f || MoveRL < 0.f) {
		const float Alpha = FMath::Abs<const float>(GetDeltaRotator(GetActorRotation(), TargetRotation).Yaw / 90.f);
		const FRotator DirectionRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, 0.107, GetDirectRotationInterp());
		if (HasAuthority())
			ClientSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
		else {
			ClientSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
			ServerSmoothSetActorRotation(FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw, DirectionRotation.Roll));
		}
	}
}

void AT_Rex::GrabBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (ABasePredator* Predator = Cast<ABasePredator>(OtherActor)) {
		if (GetMesh()->GetRelativeScale3D().X > Predator->GetMesh()->GetRelativeScale3D().X && !bIsGrab) {
			GrabPredator = Predator;
			bIsGrabActorOverlap = true;
		}
	}
}

void AT_Rex::GrabBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (ABasePredator* Predator = Cast<ABasePredator>(OtherActor)) {
		if (GetMesh()->GetRelativeScale3D().X > Predator->GetMesh()->GetRelativeScale3D().X && !bIsGrab) {
			bIsGrabActorOverlap = false;
		}
	}
}

void AT_Rex::MulticastSlopTimer_Implementation() {
	GetWorldTimerManager().SetTimer(T_AdaptingSlopeHandle, this, &AT_Rex::SlopAngle, 0.107, true);
}

bool AT_Rex::ServerPredatorRun_Validate(const float Speed) {
	return Speed >= PredatorWalkSpeed && Speed <= PredatorRunSpeed;
}

void AT_Rex::ServerSmoothSetActorRotation_Implementation(const FRotator Rotation) {
	Super::ServerSmoothSetActorRotation_Implementation(Rotation);
	const float Arc = bIsRunning ? 245.f * 2 : 245.f;
	AddMovementInput(GetActorForwardVector() * Arc);
}

void AT_Rex::ClientSmoothSetActorRotation_Implementation(const FRotator Rotation) {
	Super::ClientSmoothSetActorRotation_Implementation(Rotation);
	const float Arc = bIsRunning ? 245.f * 2 : 245.f;
	AddMovementInput(GetActorForwardVector() * Arc);
}

void AT_Rex::ServerPlayGrabMontage_Implementation(ABasePredator* GrabActor){
	MulticastPlayGrabMontage();
	if (GrabActor && !bIsGrab) {
		T_GrabDelegate.BindUFunction(this, FName("MulticastAttachToGrabPredator"), GrabActor);
		GetWorld()->GetTimerManager().SetTimer(T_GrabHandle, T_GrabDelegate, 0.9f, false);
	}
	if(GrabActor && bIsGrab){
		T_GrabDelegate.BindUFunction(this, FName("MulticastDetachToGrabPredator"), GrabActor);
		GetWorld()->GetTimerManager().SetTimer(T_GrabHandle, T_GrabDelegate, 0.3f, false);
	}
}

void AT_Rex::MulticastPlayGrabMontage_Implementation(){
	if (UAnimMontage* GrabMontage = GetPredatorGrabAnimMontage()) {
		GetMesh()->GetAnimInstance()->Montage_Play(GrabMontage);
	}
}

void AT_Rex::MulticastAttachToGrabPredator_Implementation(ABasePredator* GrabActor){
	GrabActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));
	SetIgnoreMoveInput(GrabActor);
	bIsGrab = true;
	GrabActor->PredatorSetGrab(bIsGrab);
	MulticastSetGravityScale(GrabActor, 0.f);
}

void AT_Rex::MulticastDetachToGrabPredator_Implementation(ABasePredator* GrabActor){
	GrabActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GrabActor->SetActorRotation(FRotator::ZeroRotator);
	SetEnableMoveInput(GrabActor);
	bIsGrab = false;
	GrabActor->PredatorSetGrab(bIsGrab);
	GrabPredator = nullptr;
	MulticastSetGravityScale(GrabActor, 1.f);
}

void AT_Rex::MulticastSetGravityScale_Implementation(ABasePredator* GrabActor, const float GravityScale){
	if (GrabActor && GrabActor->GetCharacterMovement()) {
		GrabActor->GetCharacterMovement()->GravityScale = GravityScale;
	}
}

void AT_Rex::MulticastPlayBiteMontage_Implementation() {
	if (!bIsBite && !bIsGrab) {
		PlayBiteMontage();
		bIsBite = true;
	}
}

void AT_Rex::MulticastPlayRoarMontage_Implementation() {
	if (!bIsRoar && !bIsGrab) {
		PlayRoarMontage();
		bIsRoar = true;
	}
}

void AT_Rex::MulticastBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) {
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

void AT_Rex::BiteDamage(ABasePredator* Predator) {
	PRINT(this,FColor::Emerald,"bIsDead = %i",Predator->GetDeadState());
	const float mMaxHealth = Predator->GetMaxHealth();
	float cCurrentHealth = Predator->GetCurrentHealth();
	if (cCurrentHealth > 0) {
		cCurrentHealth -= REX_DAMAGE;
		Predator->SetCurrentHealth(cCurrentHealth);
		Predator->OnHealthDelegate.Broadcast(cCurrentHealth, mMaxHealth);
	}
	if (cCurrentHealth <= 0) {
		Predator->GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
		Predator->GetMesh()->SetAllBodiesSimulatePhysics(true);
		Predator->SetDeadState(true);
		SetIgnoreMoveInput(Predator);
		T_RespawnPredatorDelegate.BindUFunction(this,FName("Respawn"),Predator);
		GetWorldTimerManager().SetTimer(T_RespawnPredatorHandle,T_RespawnPredatorDelegate,5.f,false);
	}
	OnCrosshairDamageDelegate.Broadcast(true);
}

bool AT_Rex::ServerPredatorDamageHealthChecker_Validate(){
	if (MaxHealth == REX_MAX_HEALTH || CurrentHealth <= REX_MAX_HEALTH && CurrentHealth >= 0)
		return true;
	return false;
}
