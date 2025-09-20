
#include "GAMESoulslike/HUD/Public/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBar::SetHealthBarColor(float InAlpha)
{
	FLinearColor NewColor = FMath::Lerp(LowHealthBarColor, HightHealthBarColor, InAlpha);
	HealthBar->SetFillColorAndOpacity(NewColor);
}
