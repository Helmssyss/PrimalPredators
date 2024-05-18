#include "AN_FootDustL.h"
#include "PrimalPredators/Characters/BasePredator.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void UAN_FootDustL::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation){
	Super::Notify(MeshComp, Animation);
	if (ABasePredator* Predator = Cast<ABasePredator>(MeshComp->GetOwner())) {
		const UStaticMeshComponent* FootprintComp = Predator->GetDecalFootLeft();
		const FTransform SpawnFootprintTransform(Predator->GetActorRotation(), FootprintComp->GetComponentLocation(), Predator->GetSpawnFootprintScale());
		if (Predator->GetPredatorType() == EPredatorType::T_REX)
			if (UParticleSystem* FootDust = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/Dinosaurs/Particle/P_FootDust"))) {
				UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld(), FootDust, SpawnFootprintTransform);
			}
	}
}
