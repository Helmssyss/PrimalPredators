#include "PterodonFootprintL.h"
#include "Components/DecalComponent.h"

APterodonFootprintL::APterodonFootprintL(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Dinosaurs/Pterodon/Material/M_pterodon_footprintL")));
}
