#pragma once

#include "CoreMinimal.h"
#include "BasePredator.h"
#include "T_Rex.generated.h"

#define BITE_MONTAGE_END 1.1f
#define ROAR_MONTAGE_END 5.f
#define REX_MAX_HEALTH   100.f
#define REX_DAMAGE       50.f

class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class PRIMALPREDATORS_API AT_Rex : public ABasePredator{
	public:
		AT_Rex();
		virtual UClass* GetPredatorClass() override;
		virtual FORCEINLINE float GetMaxCameraBoomLength() const override { return 1200.f; }
		virtual FORCEINLINE TArray<FName> GetFootBoneNames() const { return { FName("Rht_Leg_AIk004_JNT_SKL"),FName("Lft_Leg_AIk004_JNT_SKL") }; }
		virtual FORCEINLINE float GetCurrentHealth() const override { return CurrentHealth; }
		virtual FORCEINLINE float GetMaxHealth() const override { return MaxHealth; }
		virtual FORCEINLINE void SetCurrentHealth(float NewHealth) override { CurrentHealth = NewHealth; }
		virtual FORCEINLINE FVector GetSpawnFootprintScale() const override { return FVector(3.25,0.5,0.5);}
		virtual FORCEINLINE UClass* GetRightFootprintClass() const override;
		virtual FORCEINLINE UClass* GetLeftFootprintClass() const override;

	private:
		GENERATED_BODY()
	
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaSeconds) override;
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
		virtual FORCEINLINE EPredatorType GetPredatorType() const override;
		virtual bool ServerPredatorRun_Validate(const float Speed) override;
		virtual UAnimMontage* GetPredatorRoarAnimMontage() override;
		virtual UAnimMontage* GetPredatorBiteAnimMontage() override;
		virtual UAnimMontage* GetPredatorGrabAnimMontage() override;
		virtual void ServerSmoothSetActorRotation_Implementation(const FRotator Rotation) override;
		virtual void ClientSmoothSetActorRotation_Implementation(const FRotator Rotation) override;
		virtual float GetDirectRotationInterp() const override;
		virtual void MulticastPlayBiteMontage_Implementation() override;
		virtual void MulticastPlayRoarMontage_Implementation() override;
		virtual void SmoothRotation() override;
		virtual void TurnRl(const float Rate) override;
		virtual void CameraLengthUp() override;
		virtual void CameraLengthDown() override;
		virtual void MulticastSlopTimer_Implementation() override;
		virtual void SlopAngle() override;
		virtual void MulticastBiteTrace_Implementation(const FVector StartLocation, const FVector EndLocation, const float Length, FColor Color) override;
		virtual bool ServerPredatorDamageHealthChecker_Validate() override;
		
		void PlayBiteMontage();
		void PlayRoarMontage();
		void PlayGrabMontage();
		void GrabBoxSetCollision();
		void BiteDamage(ABasePredator* Predator);

		UFUNCTION()
		void GrabBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void GrabBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		UFUNCTION()
		void PlayRoarMontageEnd();

		UFUNCTION()
		void PlayBiteMontageEnd();

		UFUNCTION(Server, Unreliable)
		void ServerPlayGrabMontage(ABasePredator* GrabActor);
		void ServerPlayGrabMontage_Implementation(ABasePredator* GrabActor);

		UFUNCTION(NetMulticast, Unreliable)
		void MulticastPlayGrabMontage();
		virtual void MulticastPlayGrabMontage_Implementation();

		UFUNCTION(NetMulticast,Reliable)
		void MulticastAttachToGrabPredator(ABasePredator* GrabActor);
		void MulticastAttachToGrabPredator_Implementation(ABasePredator* GrabActor);

		UFUNCTION(NetMulticast,Reliable)
		void MulticastDetachToGrabPredator(ABasePredator* GrabActor);
		void MulticastDetachToGrabPredator_Implementation(ABasePredator* GrabActor);

		UFUNCTION(NetMulticast,Reliable)
		void MulticastSetGravityScale(ABasePredator* GrabActor, const float GravityScale = 1.f);
		void MulticastSetGravityScale_Implementation(ABasePredator* GrabActor, const float GravityScale = 1.f);
	
		UPROPERTY(EditAnywhere)
		UBoxComponent* GrabBoxComponent;

		UPROPERTY(Replicated)
		ABasePredator* GrabPredator;

		UPROPERTY(Replicated)
		bool bIsGrab;
	
		FTimerHandle T_BiteHandle;
		FTimerHandle T_RoarHandle;
		FTimerHandle T_GrabHandle;
		FTimerDelegate T_GrabDelegate;

		bool bIsBite;
		bool bIsRoar;
		bool bIsGrabActorOverlap;
		float CamDistance;
		float MaxHealth;
		float CurrentHealth;
};