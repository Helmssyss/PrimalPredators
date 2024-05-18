#pragma once

#include "CoreMinimal.h"
#include "BasePredator.h"
#include "Pterodon.generated.h"

UCLASS()
class PRIMALPREDATORS_API APterodon : public ABasePredator{
	public:
		APterodon();
		virtual  UClass* GetPredatorClass() override;
		virtual FORCEINLINE float GetMaxCameraBoomLength() const override { return 600.f; }
		virtual FORCEINLINE TArray<FName> GetFootBoneNames() const override { return { FName("Rht_Back_AIk004_JNT_SKL"),FName("Lft_Back_AIk004_JNT_SKL") }; }
		virtual FORCEINLINE float GetCurrentHealth() const override { return CurrentHealth; }
		virtual FORCEINLINE float GetMaxHealth() const override { return MaxHealth; }
		virtual FORCEINLINE void SetCurrentHealth(const float NewHealth) override { CurrentHealth = NewHealth; }
		virtual FORCEINLINE bool PredatorIsGlide() const override { return bIsGlide; }
		virtual FORCEINLINE float PredatorFlyPitchVal() const override { return FlyPitchValue; }
		virtual FORCEINLINE FVector GetSpawnFootprintScale() const override { return FVector(1.0, 0.140828, 0.108509); }
		virtual FORCEINLINE UClass* GetRightFootprintClass() const override;
		virtual FORCEINLINE UClass* GetLeftFootprintClass() const override;
		virtual FORCEINLINE bool PredatorIsFastFly() const override { return bIsFastFly; }

	private:
		GENERATED_BODY()
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaSeconds) override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		virtual void TurnRl(const float Rate) override;
		virtual bool ServerPredatorRun_Validate(const float Speed) override;
		virtual EPredatorType GetPredatorType() const override;
		virtual float GetDirectRotationInterp() const override;
		virtual void SmoothRotation() override;
		virtual void CameraLengthUp() override;
		virtual void CameraLengthDown() override;
		virtual void MulticastSlopTimer_Implementation() override;
		virtual void SlopAngle() override;
		virtual void ServerSmoothSetActorRotation_Implementation( FRotator Rotation) override;
		virtual void ClientSmoothSetActorRotation_Implementation( FRotator Rotation) override;
		virtual void Jump() override;
		virtual void RunStart() override;
		virtual void RunEnd() override;
		virtual void MoveFb(float Value) override;
	
		void SpeedFlyStart();
		void SpeedFlyEnd();
		void LookAroundAirPressed();
		void LookAroundAirReleased();
		void FreeAppendSpeed();
		void AirBrake();
		void AirBrakeReleased();
		
		UFUNCTION()
		void SpeedFlyStartWithTimer();

		UFUNCTION()
		void SpeedFlyEndWithTimer();

		UFUNCTION(Server,Unreliable,WithValidation)
		void ServerSpeedFly(const float FlySpeed);
		bool ServerSpeedFly_Validate(const float FlySpeed);
		void ServerSpeedFly_Implementation(const float FlySpeed);
	
		UFUNCTION(Server,Unreliable)
		void ServerSetMovementMode(const EMovementMode NewMovementMode);
		void ServerSetMovementMode_Implementation(const EMovementMode NewMovementMode);

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastSetMovementMode(const EMovementMode NewMovementMode);
		void MulticastSetMovementMode_Implementation(const EMovementMode NewMovementMode);

		UFUNCTION(Server,Unreliable)
		void ServerFlyingUp();
		void ServerFlyingUp_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastFlyingUp();
		void MulticastFlyingUp_Implementation();

		UFUNCTION(Server,Unreliable)
		void ServerFlyingUpFinished();
		void ServerFlyingUpFinished_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastFlyingUpFinished();
		void MulticastFlyingUpFinished_Implementation();

		UFUNCTION(Server,Unreliable)
		void ServerFlyingDown();
		void ServerFlyingDown_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastFlyingDown();
		void MulticastFlyingDown_Implementation();

		UFUNCTION(Server,Unreliable)
		void ServerFlyingDownFinished();
		void ServerFlyingDownFinished_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastFlyingDownFinished();
		void MulticastFlyingDownFinished_Implementation();
		
		UFUNCTION(Server,Unreliable)
		void ServerGlide();
		void ServerGlide_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastGlide();
		void MulticastGlide_Implementation();

		UFUNCTION(Server,Unreliable)
		void ServerResetMovement();
		void ServerResetMovement_Implementation();

		UFUNCTION(NetMulticast,Unreliable)
		void MulticastResetMovement();
		void MulticastResetMovement_Implementation();

		UFUNCTION(Server, Unreliable)
		void ServerSlowDownInAir();
		void ServerSlowDownInAir_Implementation();

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastSlowDownInAir();
		void MulticastSlowDownInAir_Implementation();

		UFUNCTION(Server, Unreliable)
		void ServerSetAirBrakeState(const bool isBrake);
		void ServerSetAirBrakeState_Implementation(const bool isBrake);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastSetAirBrakeState(const bool isBrake);
		void MulticastSetAirBrakeState_Implementation(const bool isBrake);

		UFUNCTION(Server, Unreliable)
		void ServerSetFastFlyState(bool isFastFly);
		void ServerSetFastFlyState_Implementation(bool isFastFly);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastSetFastFlyState(bool isFastFly);
		void MulticastSetFastFlyState_Implementation(bool isFastFly);

		UFUNCTION(Server, Unreliable)
		void ServerFlyingState(const bool isFlying);
		void ServerFlyingState_Implementation(const bool isFlying);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastFlyingState(const bool isFlying);
		void MulticastFlyingState_Implementation(const bool isFlying);

		UFUNCTION(Server, Unreliable)
		void ServerSetGlideState(const bool isGlide);
		void ServerSetGlideState_Implementation(const bool isGlide);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastSetGlideState(const bool isGlide);
		void MulticastSetGlideState_Implementation(const bool isGlide);

		UFUNCTION()
		void FlyingUpStart();

		UFUNCTION()
		void FlyingDownStart();

		UFUNCTION()
		void FlyingSetActorPitch();

		UFUNCTION()
		void CapsuleHitLand(UPrimitiveComponent* PrimitiveComponent, AActor* Actor, UPrimitiveComponent* PrimitiveComponent1, FVector Vector, const FHitResult& HitResult);
	
		FTimerHandle T_FlyingUpHandle;
		FTimerHandle T_FlyingDownHandle;
		FTimerHandle T_FlyingPitchHandle;
		FTimerHandle T_SpeedFlyingHandle;
		FTimerHandle T_ResetRollHandle;
		FTimerHandle T_AirBrakeHandle;

		float MaxHealth;
		float CurrentHealth;
		float FlyPitchValue;
		bool bIsFlyingUp;
		bool bIsFlying;
		bool bIsLookAround;
		bool bIsGlide;
		bool bIsBrake;
		bool bIsFastFly;
};
