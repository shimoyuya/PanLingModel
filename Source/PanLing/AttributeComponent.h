// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, class UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
// 精力改变委托，用于更新 UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStaminaChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewStamina, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** 当前生命值 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

	/** 最大生命值 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** * 应用血量变化
	 * @param Delta: 变化量 (负数表示受到伤害，正数表示治疗)
	 * @return: 实际变化了多少 (可能被 Clamp 截断)
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	/** 获取当前是否存活 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const { return Health > 0.0f; }

	/** 获取最大生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const { return MaxHealth; }
	/** 获取当前生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const { return Health; }

	/** 广播血量变化的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChanged OnHealthChanged;

/*
	体力值
*/
public:
	/** 广播体力值变化的事件 */
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;

	// 消耗精力的接口
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyStaminaChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxStamina() const { return MaxStamina; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Stamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxStamina = 100.0f;

	// 精力恢复速度 (每秒恢复多少)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float StaminaRegenRate = 15.0f;

	// 控制是否暂停恢复 (比如刚闪避完/攻击完，停顿1秒再恢复)
	bool bCanRegenStamina;
	FTimerHandle TimerHandle_StaminaRegenDelay;
};
