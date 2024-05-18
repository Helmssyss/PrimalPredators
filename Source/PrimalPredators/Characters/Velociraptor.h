#pragma once

#include "CoreMinimal.h"
#include "BasePredator.h"
#include "Velociraptor.generated.h"

#define RAPTOR_MAX_HEALTH 200.f

class APC_Play;

UCLASS()
class PRIMALPREDATORS_API AVelociraptor : public ABasePredator{
	public:
		AVelociraptor();
		virtual  UClass* GetPredatorClass() override;
		virtual FORCEINLINE float GetMaxCameraBoomLength() const override { return 600.f; }
		virtual FORCEINLINE TArray<FName> GetFootBoneNames() const override { return { FName("Rht_Back_AIk004_JNT_SKL"),FName("Lft_Back_AIk004_JNT_SKL") }; }
		virtual FORCEINLINE float GetCurrentHealth() const override { return CurrentHealth; }
		virtual FORCEINLINE float GetMaxHealth() const override { return MaxHealth; }
		virtual FORCEINLINE void SetCurrentHealth(const float NewHealth) override { CurrentHealth = NewHealth; }
		virtual FORCEINLINE FVector GetSpawnFootprintScale() const override { return FVector(1.0,0.170986,0.107352);}
		virtual FORCEINLINE UClass* GetRightFootprintClass() const override;
		virtual FORCEINLINE UClass* GetLeftFootprintClass() const override;
	
	private:
		GENERATED_BODY()

		virtual void BeginPlay() override;
		virtual void Tick(float DeltaSeconds) override;
		virtual bool ServerPredatorRun_Validate(const float Speed) override;
		virtual EPredatorType GetPredatorType() const override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		virtual UAnimMontage* GetPredatorRoarAnimMontage() override;
		virtual UAnimMontage* GetPredatorBiteAnimMontage() override;
		virtual void ServerSmoothSetActorRotation_Implementation(const FRotator Rotation) override;
		virtual void ClientSmoothSetActorRotation_Implementation(const FRotator Rotation) override;
		virtual void MulticastPlayBiteMontage_Implementation() override;
		virtual void MulticastPlayRoarMontage_Implementation() override;
		virtual float GetDirectRotationInterp() const override;
		virtual void SmoothRotation() override;
		virtual void TurnRl(const float Rate) override;
		virtual void CameraLengthUp() override;
		virtual void CameraLengthDown() override;
		virtual void MulticastSlopTimer_Implementation() override;
		virtual void SlopAngle() override;
		virtual void MulticastBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) override;
		virtual bool ServerPredatorDamageHealthChecker_Validate() override;
		virtual void Jump() override;
		virtual void MoveFb(float Value) override;

		void PlayBiteMontage();
		void PlayRoarMontage();
		void BiteDamage(ABasePredator* Predator);
	
		UFUNCTION()
		void PlayBiteMontageEnd();

		UFUNCTION()
		void PlayRoarMontageEnd();
	
		FTimerHandle T_BiteHandle;
		FTimerHandle T_RoarHandle;
		bool bIsBite;
		bool bIsRoar;
		float MaxHealth;
		float CurrentHealth;
};
