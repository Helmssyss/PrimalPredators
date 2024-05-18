#include "PterodonFootprintR.h"
#include "Components/DecalComponent.h"

APterodonFootprintR::APterodonFootprintR(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Dinosaurs/Pterodon/Material/M_pterodon_footprintR")));
}
