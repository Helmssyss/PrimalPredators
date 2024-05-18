#include "RaptorFootprintL.h"
#include "Components/DecalComponent.h"

ARaptorFootprintL::ARaptorFootprintL(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Dinosaurs/Velociraptor/Material/M_RaptorFootprintL")));
}
