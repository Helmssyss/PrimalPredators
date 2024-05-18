#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Predator.generated.h"

#define MIN_OPACITY        1.0f
#define MAX_OPACITY        0.0f
#define MIN_CAMERA_LENGTH  400.f

class UProgressBar;
class UImage;
class ABasePredator;
class APC_Play;

UCLASS()
class PRIMALPREDATORS_API UW_Predator : public UUserWidget{
	private:
		GENERATED_BODY()

		virtual void NativeOnInitialized() override;
		virtual void NativeConstruct() override;

		UFUNCTION()
		void ChangeOpacity(const float MaxCameraLength);

		UFUNCTION()
		void UpdateStamina(float CurrentStamina, float MaxStamina);

		UFUNCTION()
		void UpdateHealth(float CurrentHealth, float MaxHealth);

		UFUNCTION()
		void RespawnRefresh(ABasePredator* RespawnedCharacter);

		UFUNCTION()
		void PlayDamageAnimation(const bool isDamage);

		UPROPERTY(meta = (BindWidget))
		UImage* W_Crosshair;

		UPROPERTY(meta=(BindWidget))
		UImage* W_CrosshairHit;

		UPROPERTY(meta = (BindWidget))
		UProgressBar* W_HealthBar;

		UPROPERTY(meta = (BindWidget))
		UProgressBar* W_StaminaBar;

		UPROPERTY()
		ABasePredator* Predator;

		UPROPERTY();
		APC_Play* PlayerController;

		UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation *DamageAnimation;
};
