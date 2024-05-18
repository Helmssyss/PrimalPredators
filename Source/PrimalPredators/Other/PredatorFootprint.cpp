#include "PredatorFootprint.h"
#include "Components/DecalComponent.h"

APredatorFootprint::APredatorFootprint(){
	LifeTime = 30.f;
	PrimaryActorTick.bCanEverTick = false;

	BodyComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BodyComponent"));
	BodyComponent->SetupAttachment(GetRootComponent());
	
	DecalFootPrint = CreateDefaultSubobject<UDecalComponent>(TEXT("PredatorFootPrint"));
	DecalFootPrint->SetupAttachment(BodyComponent);
	DecalFootPrint->SetRelativeRotation(FRotator(-90.f,0.f,90.f));
}

void APredatorFootprint::BeginPlay(){
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FootLifeTimerHandle,this,&APredatorFootprint::Kill,LifeTime,false);
}

void APredatorFootprint::Kill(){
	Destroy();
}