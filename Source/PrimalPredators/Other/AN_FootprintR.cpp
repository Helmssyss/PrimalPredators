#include "AN_FootprintR.h"
#include "DrawDebugHelpers.h"
#include "PredatorFootprint.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void UAN_FootprintR::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation){
	Super::Notify(MeshComp, Animation);
	if (const ABasePredator* Predator = Cast<ABasePredator>(MeshComp->GetOwner())){
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Predator);
		const FVector Start = Predator->GetActorLocation();
		const FVector End = Start - FVector(0,0,1000);
		DrawDebugLine(MeshComp->GetWorld(),Start,End,FColor::Cyan,false,5,0,5);
		if(MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECollisionChannel::ECC_Visibility,QueryParams)){
			const UStaticMeshComponent* FootprintComp = Predator->GetDecalFootRight();
			const FTransform SpawnFootprintTransform(Predator->GetActorRotation(),FootprintComp->GetComponentLocation(),Predator->GetSpawnFootprintScale());
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			MeshComp->GetWorld()->SpawnActor<APredatorFootprint>(Predator->GetRightFootprintClass(), SpawnFootprintTransform, SpawnParameters);
		}
	}
}
