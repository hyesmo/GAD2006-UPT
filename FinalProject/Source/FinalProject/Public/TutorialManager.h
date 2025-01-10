#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialCompleted);

UCLASS()
class FINALPROJECT_API ATutorialManager : public AActor
{
    GENERATED_BODY()
    
public:    
    ATutorialManager();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintAssignable, Category = "Tutorial")
    FOnTutorialCompleted OnTutorialCompleted;

    UFUNCTION(BlueprintPure, Category = "Tutorial")
    bool IsKeyPressed(const FString& Key) const;

    UFUNCTION(BlueprintPure, Category = "Tutorial")
    bool IsTutorialComplete() const;

protected:
    virtual void BeginPlay() override;

private:
    TMap<FString, bool> PressedKeys;
    bool bTutorialComplete;

    void CheckTutorialCompletion();
}; 