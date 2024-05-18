#include "RexFootprint.h"

#include "Components/DecalComponent.h"

ARexFootprint::ARexFootprint(){
	DecalFootPrint->SetDecalMaterial(LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Dinosaurs/T-Rex/Material/M_RexFootprint")));
}
