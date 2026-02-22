// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingPlayerHUD.h"
#include "Components/ProgressBar.h"
#include "AttributeComponent.h"

void UPanLingPlayerHUD::InitializeHUD(UAttributeComponent* PlayerAttributeComp)
{
	if (PlayerAttributeComp && HealthProgressBar && StaminaProgressBar)
	{
		// 1. 绑定血量变化事件
		PlayerAttributeComp->OnHealthChanged.AddDynamic(this, &UPanLingPlayerHUD::OnHealthChanged);
		// 绑定体力值变化事件
		PlayerAttributeComp->OnStaminaChanged.AddDynamic(this, &UPanLingPlayerHUD::OnStaminaChanged);

		// 2. 初始化时，先手动更新一次进度条，防止游戏刚开始时血条是空的
		float CurrentHealth = PlayerAttributeComp->GetHealth();
		float MaxHealth = PlayerAttributeComp->GetMaxHealth();
		if (MaxHealth > 0.0f)
		{
			HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
		}
		float CurrentStamina = PlayerAttributeComp->GetStamina();
		float MaxStamina = PlayerAttributeComp->GetMaxStamina();
		if (MaxStamina > 0.0f)
		{
			StaminaProgressBar->SetPercent(CurrentStamina / MaxStamina);
		}
	}
}

void UPanLingPlayerHUD::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// 当血量发生变化时，计算新的百分比并更新进度条
	if (OwningComp && HealthProgressBar)
	{
		float MaxHealth = OwningComp->GetMaxHealth();
		if (MaxHealth > 0.0f)
		{
			// 进度条接收 0.0 到 1.0 之间的浮点数
			HealthProgressBar->SetPercent(NewHealth / MaxHealth);
		}
	}
}

void UPanLingPlayerHUD::OnStaminaChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewStamina, float Delta)
{
	if (OwningComp && StaminaProgressBar)
	{
		float MaxStamina = OwningComp->GetMaxStamina();
		if (MaxStamina > 0.0f)
		{
			// 进度条接收 0.0 到 1.0 之间的浮点数
			StaminaProgressBar->SetPercent(NewStamina / MaxStamina);
		}
	}
}

