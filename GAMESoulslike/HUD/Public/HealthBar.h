
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;

UCLASS()
class GAMESOULSLIKE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetOptional), Category = "Health Bar")
	FLinearColor HightHealthBarColor = FColor::White;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetOptional), Category = "Health Bar")
	FLinearColor LowHealthBarColor = FColor::White;

	void SetHealthBarColor(float InAlpha);
};
