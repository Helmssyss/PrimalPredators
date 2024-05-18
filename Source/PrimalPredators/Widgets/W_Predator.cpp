#include "W_Predator.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "PrimalPredators/PlayerControllers/PC_Play.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "GameFramework/SpringArmComponent.h"

void UW_Predator::NativeConstruct(){
    Super::NativeConstruct();
    W_CrosshairHit->SetVisibility(ESlateVisibility::Hidden);
}

void UW_Predator::NativeOnInitialized(){
	Super::NativeOnInitialized();

    PlayerController = GetOwningPlayer<APC_Play>();
    if (PlayerController) {
        Predator = Cast<ABasePredator>(PlayerController->GetPawn());
        if (Predator) {
            Predator->OnCrosshairOpacityDelegate.AddDynamic(this, &UW_Predator::ChangeOpacity);
            Predator->OnStaminaDelegate.AddDynamic(this, &UW_Predator::UpdateStamina);
            Predator->OnHealthDelegate.AddDynamic(this, &UW_Predator::UpdateHealth);
            Predator->OnCrosshairDamageDelegate.AddDynamic(this, &UW_Predator::PlayDamageAnimation);
        }
    }
}

void UW_Predator::ChangeOpacity(const float MaxCameraLength) {
    if (PlayerController) {
        if (Predator) {
            const float CameraLength = FMath::Clamp(Predator->GetCameraBoom()->TargetArmLength, MIN_OPACITY, MaxCameraLength);
            const float Opacity = FMath::GetMappedRangeValueClamped(FVector2D(MIN_CAMERA_LENGTH, MaxCameraLength), FVector2D(MIN_OPACITY, MAX_OPACITY), CameraLength);
            W_Crosshair->SetOpacity(Opacity);
        }
    }
}

void UW_Predator::RespawnRefresh(ABasePredator* RespawnedCharacter){
    Predator = RespawnedCharacter;
    Predator->OnStaminaDelegate.Clear();
    Predator->OnHealthDelegate.Clear();
    Predator->OnCrosshairOpacityDelegate.Clear();
    Predator->OnCrosshairDamageDelegate.Clear();

    W_StaminaBar->SetPercent(1.f);
    W_HealthBar->SetPercent(1.f);

    Predator->OnCrosshairOpacityDelegate.AddDynamic(this, &UW_Predator::ChangeOpacity);
    Predator->OnCrosshairDamageDelegate.AddDynamic(this, &UW_Predator::PlayDamageAnimation);
    Predator->OnStaminaDelegate.AddDynamic(this, &UW_Predator::UpdateStamina);
    Predator->OnHealthDelegate.AddDynamic(this, &UW_Predator::UpdateHealth);
}

void UW_Predator::PlayDamageAnimation(const bool isDamage){
    if(isDamage){
        W_CrosshairHit->SetVisibility(ESlateVisibility::Visible);
        PlayAnimation(DamageAnimation);
    }
}

void UW_Predator::UpdateStamina(float CurrentStamina, float MaxStamina){
    const float Percent = CurrentStamina / MaxStamina;
    W_StaminaBar->SetPercent(Percent);
}

void UW_Predator::UpdateHealth(float CurrentHealth, float MaxHealth){
    const float Percent = CurrentHealth / MaxHealth;
    W_HealthBar->SetPercent(Percent);
}