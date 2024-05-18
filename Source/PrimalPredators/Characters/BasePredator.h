#pragma once

#include "CoreMinimal.h"
#include "PredatorSettings.h"
#include "GameFramework/Character.h"
#include "BasePredator.generated.h"

#define PRINT(obj,color,fmt,...)    UKismetSystemLibrary::PrintString(obj,FString::Printf(TEXT(fmt),__VA_ARGS__),true,true,FLinearColor(color),5)
#define MAX_HEAD_ROT_YAW            140

class UInputComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class USphereComponent;
class APC_Play;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaDelegate, float, CurrentStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthDelegate, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrosshairOpacityDelegate, const float, MaxCameraLength);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrosshairDamageDelegate, const bool, isDamage);

UCLASS()
class PRIMALPREDATORS_API ABasePredator : public ACharacter{
	public:
		ABasePredator();
	
		virtual FORCEINLINE EPredatorType GetPredatorType() const { return EPredatorType::EMPTY; }
		virtual FORCEINLINE float GetPredatorRunSpeed() const { return PredatorRunSpeed; }
		virtual FORCEINLINE float GetPredatorWalkSpeed() const { return PredatorWalkSpeed; }
		virtual FORCEINLINE UClass* GetPredatorClass() { return StaticClass(); }
		virtual FORCEINLINE float GetMaxCameraBoomLength() const { return -1.f; }
		virtual FORCEINLINE TArray<FName> GetFootBoneNames() const { return {}; }
		virtual FORCEINLINE float GetCurrentHealth() const { return -1; }
		virtual FORCEINLINE float GetMaxHealth() const { return -1; }
		virtual FORCEINLINE void SetCurrentHealth(const float NewHealth) {}
		virtual FORCEINLINE FVector GetSpawnFootprintScale() const { return FVector::ZeroVector;}
		virtual FORCEINLINE UClass* GetRightFootprintClass() const {return nullptr;}
		virtual FORCEINLINE UClass* GetLeftFootprintClass() const {return nullptr;}
		virtual FORCEINLINE bool PredatorIsGlide() const { return false; }
		virtual FORCEINLINE bool PredatorIsFastFly() const { return false; }
		virtual FORCEINLINE float PredatorFlyPitchVal() const { return 0.f; }

		FORCEINLINE void PredatorSetGrab(bool isGrab) { bIsGrabbed = isGrab; }
		FORCEINLINE bool PredatorHasGrabbed() const { return bIsGrabbed; }
		FORCEINLINE FRotator GetHeadRotator() const { return HeadRotation; }
		FORCEINLINE static FRotator GetDeltaRotator(const FRotator &A, const FRotator &B) {return (A - B).GetNormalized();}
		FORCEINLINE static FVector  GetForwardVector(const FRotator InRot) {return InRot.Vector();} 
		FORCEINLINE static FVector GetRightVector(const FRotator InRot) {return FRotationMatrix(InRot).GetScaledAxis(EAxis::Y);}
		FORCEINLINE static float SelectFloat(const float A, const float B, const bool bSelectA) {return bSelectA ? A : B;}
		FORCEINLINE FVector SelectVector(const FVector& A, const FVector& B, const bool& bSelectA) const { return bSelectA ? A : B; }
		FORCEINLINE static FRotator MakeRotFromX(const FVector& X) {return FRotationMatrix::MakeFromX(X).Rotator();}
		FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
		FORCEINLINE FRotator FindLookAtRotation(const FVector& Start, const FVector& Target) const { return FRotationMatrix::MakeFromX(Target - Start).Rotator(); }
		FORCEINLINE void SetDeadState(const bool isDead) {bIsDead = isDead;}
		FORCEINLINE bool GetDeadState() const {return bIsDead;}
		FORCEINLINE UStaticMeshComponent* GetDecalFootRight() const {return DecalFootRight;}
		FORCEINLINE UStaticMeshComponent* GetDecalFootLeft() const {return DecalFootLeft;}
		FORCEINLINE static FRotator MakeRotFromZ(const FVector& Z) {return FRotationMatrix::MakeFromZ(Z).Rotator();}
	
		FRotator DirectionModifier(const TEnumAsByte<EMovementMode> MovementMode) const;
		void SetCollisionContainer() const;
		void SetIgnoreMoveInput(const ABasePredator* Predator) const;
		void SetEnableMoveInput(const ABasePredator* Predator) const;

		UPROPERTY(Replicated)
		float YawInput;

		UPROPERTY(Replicated)
		float PitchInput;

		UPROPERTY(Replicated)
		bool bIsRunning;

		FRotator GroundSlopeForward;
		FOnStaminaDelegate OnStaminaDelegate;
		FOnHealthDelegate OnHealthDelegate;
		FOnCrosshairOpacityDelegate OnCrosshairOpacityDelegate;
		FOnCrosshairDamageDelegate OnCrosshairDamageDelegate;

	protected:
		virtual void BeginPlay() override;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
		virtual UAnimMontage* GetPredatorRoarAnimMontage() { return nullptr; }
		virtual UAnimMontage* GetPredatorBiteAnimMontage() { return nullptr; }
		virtual UAnimMontage* GetPredatorGrabAnimMontage() { return nullptr; }
		virtual FORCEINLINE float GetDirectRotationInterp() const { return -1; }
		virtual void SmoothRotation(){}
		virtual void CameraLengthUp(){}
		virtual void CameraLengthDown(){}
		virtual void TurnRl(const float Rate);
		virtual void RunStart();
		virtual void RunEnd();
		virtual void MoveFb(float Value);
	
		void MoveRl(const float Value);
		void LookUp(const float Rate);
		void DirectionTrace(const FVector StartLocation, const uint16 Length, const FColor Color);
		bool CreateLineTraceForSlope(FHitResult& HitResult, const short Length, const FVector VectorDirection = FVector::ZeroVector);
		void BiteTrace(const float Length, FColor Color);
		void ImpactBloodParticle(const FHitResult& HitResult, const ABasePredator* ImpactActor) const;

		UFUNCTION()
		void Respawn(ABasePredator* Predator);

		UFUNCTION()
		virtual void SlopAngle() {}
	
		UFUNCTION(Server,Unreliable,WithValidation)
		void ServerPredatorRun(const float Speed);
		virtual bool ServerPredatorRun_Validate(const float Speed);
		void ServerPredatorRun_Implementation(const float Speed);
	
		UFUNCTION(Server,Unreliable,WithValidation)
		void ServerAimOffset(const float Pitch, const float Yaw);
		bool ServerAimOffset_Validate(const float Pitch, const float Yaw);
		void ServerAimOffset_Implementation(const float Pitch, const float Yaw);

		UFUNCTION(Client,Unreliable,WithValidation)
		void ClientAimOffset(const float Pitch, const float Yaw);
		bool ClientAimOffset_Validate(const float Pitch, const float Yaw);
		void ClientAimOffset_Implementation(const float Pitch, const float Yaw);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastSlopTimer();
		virtual void MulticastSlopTimer_Implementation(){}

		UFUNCTION(Server,Unreliable)
		void ServerSmoothSetActorRotation( FRotator Rotation);
		virtual void ServerSmoothSetActorRotation_Implementation( FRotator Rotation);

		UFUNCTION(Client, Unreliable)
		void ClientSmoothSetActorRotation( FRotator Rotation);
		virtual void ClientSmoothSetActorRotation_Implementation( FRotator Rotation);

		UFUNCTION(Server,Unreliable)
		void ServerPlayRoarMontage();
		void ServerPlayRoarMontage_Implementation();

		UFUNCTION(Server, Unreliable)
		void ServerPlayBiteMontage();
		void ServerPlayBiteMontage_Implementation();

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastPlayBiteMontage();
		virtual void MulticastPlayBiteMontage_Implementation() {}

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastPlayRoarMontage();
		virtual void MulticastPlayRoarMontage_Implementation() {}

		UFUNCTION(Server,Unreliable,WithValidation)
		void ServerPredatorDamageHealthChecker();
		virtual bool ServerPredatorDamageHealthChecker_Validate() { return true; }
		void ServerPredatorDamageHealthChecker_Implementation(){}

		UFUNCTION(Server, Unreliable)
		void ServerBiteTrace(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color);
		void ServerBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastBiteTrace(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color);
		virtual void MulticastBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) {}
	
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent *CameraBoom;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent *Camera;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USphereComponent* BodyHitSphere;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DecalFootPrint, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* DecalFootLeft;
	
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DecalFootPrint, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* DecalFootRight;
	
		UPROPERTY(Replicated)
		bool bIsDead;

		UPROPERTY(Replicated)
		bool bIsGrabbed;
	
		float PredatorRunSpeed;
		float PredatorWalkSpeed;
		float PredatorMaxFlySpeed;
		float PredatorDefaultFlySpeed;
		float PredatorGlideSpeed;
	
		FRotator ResultAimOffsetRotator;
		FRotator HeadRotation;
	
		FTimerHandle T_AdaptingSlopeHandle;
		FTimerHandle T_RunHandle;
		FTimerHandle T_RespawnPredatorHandle;
		FTimerDelegate T_RespawnPredatorDelegate;

		FHitResult SlopeAngleHitForwardResult;
		FHitResult SlopeAngleHitBackwardResult;
	
	private:
		GENERATED_BODY()

		void AimOffset();
		
		UFUNCTION()
		void RunWithTimerStart();

		UFUNCTION()
		void RunWithTimerEnd();

		void HitSphereSetCollision();
	
		UFUNCTION()
		void StaminaRefill();

	#pragma region TEST_CHANGE_CHAR
		void TestChangeChar();
	#pragma region TEST_CHANGE_CHAR
	
		EPredatorType PredatorType;
		bool bIsMoveF;
		bool bIsStaminaDecreased;
		float CurrentStamina;
		float MaxStamina;
		FTimerHandle T_StaminaHandle;
};