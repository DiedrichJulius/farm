#include "farmInteractableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"


AfarmInteractableActor::AfarmInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false; // No need to tick for now

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	InteractionPrompt = FText::FromString("Press E");
	bIsFocused = false;
}

void AfarmInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AfarmInteractableActor::OnFocused()
{
	if (!bIsFocused)
	{
		bIsFocused = true;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, TEXT("SHOW: Press E"));
	}
}

void AfarmInteractableActor::OnUnfocused()
{
	if (bIsFocused)
	{
		bIsFocused = false;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("HIDE: Press E"));
	}
}
