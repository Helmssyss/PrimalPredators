#include "RaptorFootprintR.h"
#include "Components/DecalComponent.h"

ARaptorFootprintR::ARaptorFootprintR(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Dinosaurs/Velociraptor/Material/M_RaptorFootprintR")));
}
