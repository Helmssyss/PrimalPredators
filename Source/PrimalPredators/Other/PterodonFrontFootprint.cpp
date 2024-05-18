#include "PterodonFrontFootprint.h"
#include "Components/DecalComponent.h"

APterodonFrontFootprint::APterodonFrontFootprint(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Dinosaurs/Pterodon/Material/M_pterodon_frontFootprint")));
}
