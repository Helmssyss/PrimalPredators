#include "BasePredator.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PrimalPredators/GameModes/GM_Play.h"
#include "PrimalPredators/PlayerControllers/PC_Play.h"
#include "Blueprint/UserWidget.h"
#include "PrimalPredators/Other/BloodDecal.h"

ABasePredator::ABasePredator(){
	PrimaryActorTick.bCanEverTick = true;
	bIsDead = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	BodyHitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BodyHitSphere"));

	DecalFootRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DecalFootRight"));
	DecalFootRight->SetHiddenInGame(true);
	DecalFootRight->SetCastShadow(false);
	DecalFootLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DecalFootLeft"));
	DecalFootLeft->SetHiddenInGame(true);
	DecalFootLeft->SetCastShadow(false);

	GetMesh()->SetIsReplicated(true);
	SetCollisionContainer();
	HitSphereSetCollision();
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
	bReplicates = true;
	CurrentStamina = 500.f;
	MaxStamina = 500.f;
}

void ABasePredator::BeginPlay(){
	Super::BeginPlay();
}

void ABasePredator::EndPlay(const EEndPlayReason::Type EndPlayReason){
	Super::EndPlay(EndPlayReason);
}

void ABasePredator::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	//ServerPredatorDamageHealthChecker();
}

void ABasePredator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Run",IE_Pressed,this,&ABasePredator::RunStart);
	PlayerInputComponent->BindAction("Run",IE_Released,this,&ABasePredator::RunEnd);
	PlayerInputComponent->BindAction("Roar", IE_Pressed, this, &ABasePredator::ServerPlayRoarMontage);
	PlayerInputComponent->BindAction("Bite", IE_Pressed, this, &ABasePredator::ServerPlayBiteMontage);
	PlayerInputComponent->BindAction("CameraLengthUp",IE_Pressed, this, &ABasePredator::CameraLengthUp);
	PlayerInputComponent->BindAction("CameraLengthDown",IE_Pressed, this, &ABasePredator::CameraLengthDown);
	PlayerInputComponent->BindAction("Test - Change Char",IE_Pressed,this,&ABasePredator::TestChangeChar);
	
	PlayerInputComponent->BindAxis("MoveFB", this, &ABasePredator::MoveFb);
	PlayerInputComponent->BindAxis("MoveRL", this, &ABasePredator::MoveRl);
	PlayerInputComponent->BindAxis("TurnRL", this, &ABasePredator::TurnRl);
	PlayerInputComponent->BindAxis("LookUp", this, &ABasePredator::LookUp);
}

void ABasePredator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasePredator, YawInput);
	DOREPLIFETIME(ABasePredator, PitchInput);
	DOREPLIFETIME(ABasePredator, bIsRunning);
	DOREPLIFETIME(ABasePredator, bIsDead);
	DOREPLIFETIME(ABasePredator, bIsGrabbed);
}

void ABasePredator::SetCollisionContainer() const{
	FCollisionResponseContainer CapsuleCollisionResponseContainer;
	CapsuleCollisionResponseContainer.SetResponse(ECC_Visibility, ECR_Ignore);
	CapsuleCollisionResponseContainer.SetResponse(ECC_Camera, ECR_Ignore);
	CapsuleCollisionResponseContainer.SetResponse(ECC_WorldStatic, ECR_Block);
	CapsuleCollisionResponseContainer.SetResponse(ECC_WorldDynamic, ECR_Block);
	CapsuleCollisionResponseContainer.SetResponse(ECC_Pawn, ECR_Ignore);
	CapsuleCollisionResponseContainer.SetResponse(ECC_PhysicsBody, ECR_Block);
	CapsuleCollisionResponseContainer.SetResponse(ECC_Vehicle, ECR_Block);
	CapsuleCollisionResponseContainer.SetResponse(ECC_Destructible, ECR_Block);

	FCollisionResponseContainer MeshCollisionResponseContainer;
	MeshCollisionResponseContainer.SetResponse(ECC_Visibility, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_Camera, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_WorldStatic, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_WorldDynamic, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_Pawn, ECR_Ignore);
	MeshCollisionResponseContainer.SetResponse(ECC_PhysicsBody, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_Vehicle, ECR_Block);
	MeshCollisionResponseContainer.SetResponse(ECC_Destructible, ECR_Block);
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannels(MeshCollisionResponseContainer);
	GetMesh()->SetCollisionProfileName("Custom");
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToChannels(CapsuleCollisionResponseContainer);
	GetCapsuleComponent()->SetCollisionProfileName("Custom");
}

void ABasePredator::HitSphereSetCollision() {
	FCollisionResponseContainer HitSphereResponseContainer;
	HitSphereResponseContainer.SetResponse(ECC_Visibility, ECR_Block);
	HitSphereResponseContainer.SetResponse(ECC_Camera, ECR_Overlap);
	HitSphereResponseContainer.SetResponse(ECC_WorldStatic, ECR_Overlap);
	HitSphereResponseContainer.SetResponse(ECC_WorldDynamic, ECR_Overlap);
	HitSphereResponseContainer.SetResponse(ECC_Pawn, ECR_Ignore);
	HitSphereResponseContainer.SetResponse(ECC_PhysicsBody, ECR_Overlap);
	HitSphereResponseContainer.SetResponse(ECC_Vehicle, ECR_Ignore);
	HitSphereResponseContainer.SetResponse(ECC_Destructible, ECR_Ignore);

	BodyHitSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BodyHitSphere->SetCollisionResponseToChannels(HitSphereResponseContainer);
	BodyHitSphere->SetCollisionProfileName("Custom");
}

#pragma region TEST_CHANGE_CHAR
void ABasePredator::TestChangeChar(){
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * 2000;
	GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility,QueryParams);
	DrawDebugLine(GetWorld(),Start,End,FColor::Red,false,5,0,10);
	if(ABasePredator* P = Cast<ABasePredator>(HitResult.GetActor())){
		PRINT(this,FColor::Orange,"KARAKTER -> %s",*P->GetName());
		DrawDebugBox(GetWorld(),HitResult.ImpactPoint,FVector(10,10,10),FColor::Red,false,5,0,3);
		GetController()->Possess(P);
	}
}
#pragma endregion TEST_CHANGE_CHAR

void ABasePredator::MoveFb(const float Value){
	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABasePredator::MoveRl(const float Value) {
	if (Controller != nullptr) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABasePredator::TurnRl(const float Rate){
	YawInput = Rate * 45.f * GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(YawInput);
	AimOffset();
}

void ABasePredator::LookUp(const float Rate){
	PitchInput = Rate * 45.f * GetWorld()->GetDeltaSeconds();
	AddControllerPitchInput(PitchInput);
	AimOffset();
}

void ABasePredator::RunStart(){
	GetWorldTimerManager().SetTimer(T_RunHandle,this,&ABasePredator::RunWithTimerStart,0.107,true);
}

void ABasePredator::RunEnd(){
	GetWorldTimerManager().ClearTimer(T_RunHandle);
	GetWorldTimerManager().SetTimer(T_RunHandle,this,&ABasePredator::RunWithTimerEnd,0.107,true);
}

void ABasePredator::RunWithTimerStart(){
	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed,PredatorRunSpeed,GetWorld()->GetDeltaSeconds(),25);
	ServerPredatorRun(GetCharacterMovement()->MaxWalkSpeed);
	bIsRunning = true;
	if (GetCharacterMovement()->Velocity.Size() > 0 && !GetCharacterMovement()->IsFalling()) {
		const float LoopPerSecond = 0.107f;
		const float StaminaPerSecond = 15.f;
		CurrentStamina = FMath::Clamp(CurrentStamina - (LoopPerSecond * StaminaPerSecond), 0.f, MaxStamina);
		OnStaminaDelegate.Broadcast(CurrentStamina, MaxStamina);
		bIsStaminaDecreased = true;
	}
	if (CurrentStamina <= 0)
		RunEnd();
}

void ABasePredator::RunWithTimerEnd(){
	if (bIsStaminaDecreased) {
		GetWorldTimerManager().SetTimer(T_StaminaHandle, this, &ABasePredator::StaminaRefill, 0.107, true);
	}
	
	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed,PredatorWalkSpeed,GetWorld()->GetDeltaSeconds(),25);
	ServerPredatorRun(GetCharacterMovement()->MaxWalkSpeed);
	bIsRunning = false;
	bIsStaminaDecreased = false;
	if(GetCharacterMovement()->MaxWalkSpeed - PredatorWalkSpeed == 0.0f){
		GetWorldTimerManager().ClearTimer(T_RunHandle);
	}
}

void ABasePredator::StaminaRefill() {
	if (!bIsStaminaDecreased) {
		const float LoopPerSecond = 0.107f;
		const float StaminaPerSecond = 5.f;
		CurrentStamina = FMath::Clamp(CurrentStamina + (LoopPerSecond * StaminaPerSecond), 0.f, MaxStamina);
		OnStaminaDelegate.Broadcast(CurrentStamina, MaxStamina);
		if (CurrentStamina == MaxStamina) {
			GetWorldTimerManager().ClearTimer(T_StaminaHandle);
		}
	}else{
		GetWorldTimerManager().ClearTimer(T_StaminaHandle);
	}
}

void ABasePredator::AimOffset(){
	HeadRotation = GetDeltaRotator(GetControlRotation(),GetActorRotation());
	if(HasAuthority()){
		ClientAimOffset(HeadRotation.Pitch, HeadRotation.Yaw);
	}else{
		ClientAimOffset(HeadRotation.Pitch, HeadRotation.Yaw);
		ServerAimOffset(HeadRotation.Pitch, HeadRotation.Yaw);
	}
}

bool ABasePredator::CreateLineTraceForSlope(FHitResult &HitResult, const short Length,const FVector VectorDirection){
	const FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector Distance;
	FVector End;
	
	if(VectorDirection == FVector::ZeroVector){
		Distance = GetCapsuleComponent()->GetForwardVector() * Length + FVector(0.f,0.f,-600.f);
		End = Start + Distance;	
	}else{
		Distance = VectorDirection * Length + FVector(0.f,0.f,-600.f);
		End = Start + Distance;
	}

	const bool bTrace = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Pawn);

	DrawDebugLine(GetWorld(), Start, End, FColor::Magenta,false,-1,0,3);
	DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Red);

	return bTrace;
}

FRotator ABasePredator::DirectionModifier(const TEnumAsByte<EMovementMode> MovementMode) const {
	FRotator TargetRotation;
	float ClampRoll;
	const float MoveFB = GetInputAxisValue("MoveFB");
	const float MoveRL = GetInputAxisValue("MoveRL");
	const float AngleYaw = FMath::RadiansToDegrees(FMath::Atan2(MoveRL, MoveFB));
	
	switch (MovementMode){
		case MOVE_Walking:
			TargetRotation = FRotator(0.f, GetControlRotation().Yaw + AngleYaw, 0.f);
			return TargetRotation;
		
		case MOVE_Flying:
			ClampRoll = FMath::Clamp<float>(GetDeltaRotator(GetBaseAimRotation(),GetActorRotation()).Yaw,-45,45);
			TargetRotation = FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw + AngleYaw, ClampRoll);
			return TargetRotation;

		default: return FRotator::ZeroRotator;
	}
}

void ABasePredator::DirectionTrace(const FVector StartLocation, const uint16 Length,const FColor Color){
	const FVector Start = GetActorLocation();
	const FVector End = Start + StartLocation * Length;
	DrawDebugDirectionalArrow(GetWorld(), Start, End, 15, Color, false, -1, 0, 15);
}

void ABasePredator::BiteTrace(const float Length, FColor Color){
	const FVector StartLocation = CameraBoom->GetComponentLocation();
	const FVector EndLocation = StartLocation + Camera->GetForwardVector() * Length;
	ServerBiteTrace(StartLocation, EndLocation, Length, Color);
}

void ABasePredator::ImpactBloodParticle(const FHitResult& HitResult,const ABasePredator* ImpactActor) const{
	/*
	 * UNiagaraSystem* BloodParticle = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/Dinosaurs/Material/P_E_BloodSplash1"));
	 * UNiagaraSystem* BloodSmokeParticle = LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/Dinosaurs/Material/P_E_Blood_SmokeDroplets1"));
	 * UNiagaraSystem* BloodMeshBloodParticle = LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/Dinosaurs/Material/P_Splash_SkMesh1"));
	 * const TArray<UNiagaraSystem*> BloodParticles = {BloodParticle, BloodSmokeParticle,BloodMeshBloodParticle};
	 * for (UNiagaraSystem* Particle : BloodParticles){
	 *	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),Particle,HitResult.ImpactPoint,FRotator::ZeroRotator,FVector(1,1,1));	
	 * }
	 */
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FTransform SpawnTransform(MakeRotFromZ(HitResult.ImpactNormal),HitResult.ImpactPoint,FVector(0.5,0.5,0.5));
	ABloodDecal* Blood = GetWorld()->SpawnActor<ABloodDecal>(ABloodDecal::StaticClass(),SpawnTransform,SpawnParameters);
	if(Blood)
		Blood->AttachToComponent(ImpactActor->GetMesh(),FAttachmentTransformRules::KeepWorldTransform,ImpactActor->GetMesh()->FindClosestBone(HitResult.Location));
}

void ABasePredator::SetIgnoreMoveInput(const ABasePredator* Predator) const{
	if (APC_Play* GrabController = Cast<APC_Play>(Predator->GetController())){
		GrabController->SetIgnoreMoveInput(true);
	}
}

void ABasePredator::SetEnableMoveInput(const ABasePredator* Predator) const{
	if (APC_Play* GrabController = Cast<APC_Play>(Predator->GetController())){
		GrabController->SetIgnoreMoveInput(false);
	}
}

void ABasePredator::Respawn(ABasePredator* Predator){
	PRINT(this,FColor::Emerald,"Respawn Methodu");
	if (const AGM_Play* GameMode = Cast<AGM_Play>(GetWorld()->GetAuthGameMode())){
		if (APC_Play* PController = Cast<APC_Play>(Predator->GetController())){
			GameMode->RespawnPredator(PController,Predator);
			PRINT(this,FColor::Purple,"Respawn");
		}
	}
}

void ABasePredator::ServerBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) {
	MulticastBiteTrace(StartLocation, EndLocation, Length, Color);
}

bool ABasePredator::ServerPredatorRun_Validate(const float Speed){
	return Speed >= PredatorWalkSpeed && Speed <= PredatorRunSpeed;
}

void ABasePredator::ServerPredatorRun_Implementation(const float Speed){
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

bool ABasePredator::ServerAimOffset_Validate(const float Pitch, const float Yaw) {
	return true;
}

void ABasePredator::ServerAimOffset_Implementation(const float Pitch, const float Yaw){
	PitchInput = Pitch;
	YawInput = Yaw;
}

bool ABasePredator::ClientAimOffset_Validate(const float Pitch, const float Yaw) {
	return true;
}

void ABasePredator::ClientAimOffset_Implementation(const float Pitch, const float Yaw){
	PitchInput = Pitch;
	YawInput = Yaw;
}
	
void ABasePredator::ServerSmoothSetActorRotation_Implementation( FRotator Rotation) {
	SetActorRotation(Rotation);
}

void ABasePredator::ClientSmoothSetActorRotation_Implementation( FRotator Rotation) {
	SetActorRotation(Rotation);
}
	
void ABasePredator::ServerPlayRoarMontage_Implementation() {
	 MulticastPlayRoarMontage();
}

void ABasePredator::ServerPlayBiteMontage_Implementation(){
	 MulticastPlayBiteMontage();
}