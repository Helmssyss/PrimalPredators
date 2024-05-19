#pragma once

#include "CoreMinimal.h"
#include "PredatorSettings.generated.h"

class ABasePredator;

UENUM()
enum class EPredatorType : uint8{
	T_REX        UMETA(DisplayName="T-Rex"),
	Velociraptor UMETA(DisplayName="Velociraptor"),
	Pterodon     UMETA(DisplayName="Pterodon"),
	EMPTY        UMETA(DisplayName="Empty")
};

USTRUCT()
struct FPredatorSettings{
	GENERATED_BODY()

	UPROPERTY()
	UClass* PredatorClass;

	UPROPERTY()
	FString PredatorPlayerName;
};
