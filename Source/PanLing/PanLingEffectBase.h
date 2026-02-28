// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PanLingEffectBase.generated.h"

class UAttributeComponent;

/**
 * 自定义 Buff/状态 系统的基类
 */
UCLASS(Blueprintable, BlueprintType)
class PANLING_API UPanLingEffectBase : public UObject
{
	GENERATED_BODY()
	
public:
	UPanLingEffectBase();

	// 重写 GetWorld 以支持在蓝图子类中使用世界上下文相关节点（如生成特效、音效等）
	virtual UWorld* GetWorld() const override;

	/* --- 核心配置参数（供策划在蓝图中配置） --- */

	// Buff 持续时间 (小于等于 0 表示永久存在，直到被手动移除)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PanLing|Effect")
	float Duration;

	// 周期性触发的时间间隔 (例如每 1 秒扣一次血。小于等于 0 表示不周期触发)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PanLing|Effect")
	float TickInterval;

	/* --- 运行时状态 --- */

	UPROPERTY(BlueprintReadOnly, Category = "PanLing|Effect")
	float TimeRemaining;

	UPROPERTY(BlueprintReadOnly, Category = "PanLing|Effect")
	float TimeSinceLastTick;

	// 记录该 Buff 当前挂载在哪个属性组件上
	UPROPERTY(BlueprintReadOnly, Category = "PanLing|Effect")
	UAttributeComponent* TargetAttributeComp;

	/* --- C++ 核心逻辑接口 --- */

	void StartEffect(UAttributeComponent* Target);
	bool UpdateEffect(float DeltaTime); // 返回 false 表示该 Buff 已过期需要被销毁
	void EndEffect();

protected:
	/* --- 供蓝图重写的事件接口 --- */

	// Buff 被添加到角色身上时触发（适合做：加攻击力、播放中毒特效）
	UFUNCTION(BlueprintNativeEvent, Category = "PanLing|Effect")
	void OnApply();

	// 周期性触发（适合做：持续扣血、持续回血）
	UFUNCTION(BlueprintNativeEvent, Category = "PanLing|Effect")
	void OnTick();

	// Buff 结束或被移除时触发（适合做：还原攻击力、销毁中毒特效）
	UFUNCTION(BlueprintNativeEvent, Category = "PanLing|Effect")
	void OnRemove();

};
