#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPredator.generated.h"

class USpringArmComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UCapsuleComponent;

UCLASS()
class PRIMALPREDATORS_API ALobbyPredator : public APawn{

	public:
		ALobbyPredator();

		FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
		FORCEINLINE USkeletalMeshComponent* GetMesh() const { return SkeletalMesh; }
		FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	private:
		GENERATED_BODY()

		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		void TurnRl(const float Rate);
		void LookUp(const float Rate);

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkeletalMesh, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* SkeletalMesh;
		
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Capsule, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* Capsule;
};
