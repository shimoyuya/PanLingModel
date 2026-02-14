// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, class UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

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
	bool IsAlive() const;

	/** 获取最大生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;
	/** 获取当前生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	/** 广播血量变化的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChanged OnHealthChanged;
};
