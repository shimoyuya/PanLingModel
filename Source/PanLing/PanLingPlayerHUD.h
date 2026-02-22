// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingPlayerHUD.generated.h"

class UProgressBar;
class UAttributeComponent;

UCLASS()
class PANLING_API UPanLingPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// meta = (BindWidget) 是一个魔法宏！
	// 它会强制要求你的蓝图里必须有一个名字完全一样的进度条组件，然后 C++ 会自动把它绑定过来。
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

public:
	// 初始化函数，用来把玩家的属性组件传进来
	UFUNCTION(BlueprintCallable)
	void InitializeHUD(UAttributeComponent* PlayerAttributeComp);

protected:
	// 监听血量变化的毁调函数（签名必须和 Delegate 一致）
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta);

	// 监听体力值变化的函数
	UFUNCTION()
	void OnStaminaChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewStamina, float Delta);
};
