#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "farmInteractableActor.generated.h"

UCLASS()
class FARM_API AfarmInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	AfarmInteractableActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionPrompt;

	void OnFocused();
	void OnUnfocused();

private:
	bool bIsFocused;
};
