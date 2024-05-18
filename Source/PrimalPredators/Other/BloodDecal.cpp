#include "BloodDecal.h"
#include "Components/DecalComponent.h"

ABloodDecal::ABloodDecal(){
	GetDecal()->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Dinosaurs/Material/M_BloodDecal")));
}
