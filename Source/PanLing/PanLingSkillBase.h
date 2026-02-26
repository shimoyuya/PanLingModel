// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PanLingSkillBase.generated.h"

class ACharacter;
class UAnimMontage;

/**
 * 技能基类：所有具体技能（如火球术、旋风斩）都继承自此类
 * Blueprintable 允许我们在蓝图中创建它的子类
 */
UCLASS(Blueprintable, BlueprintType)
class PANLING_API UPanLingSkillBase : public UObject
{
	GENERATED_BODY()
	
public:
	// 技能名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
	FName SkillName;

	// 冷却时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
	float CooldownDuration = 3.0f;

	// 消耗的精力值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
	float StaminaCost = 20.0f;

	// 释放技能时播放的蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	UAnimMontage* CastMontage;

	// 技能图标 (未来给 UI 留的接口)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Info")
	class UTexture2D* SkillIcon;

	// 当前是否处于冷却中
	UPROPERTY(BlueprintReadOnly, Category = "Skill|State")
	bool bIsOnCooldown = false;

public:
	// 重写 GetWorld，这是 UObject 能使用定时器和生成 Actor 的关键！
	virtual UWorld* GetWorld() const override;

	// 检查是否满足释放条件 (比如精力是否足够，是否在冷却中)
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual bool CanCast(ACharacter* Caster);

	// 执行技能的核心入口
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void CastSkill(ACharacter* Caster);

protected:
	// 供蓝图子类重写的具体技能表现 (比如发射火球、生成特效)
	// BlueprintNativeEvent 允许 C++ 有默认实现，同时蓝图也能重写
	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	void OnSkillExecuted(ACharacter* Caster);
	// 声明函数的实现版本
	void OnSkillExecuted_Implementation(ACharacter* Executor);

	// 冷却结束的回调
	void FinishCooldown();

private:
	// 管理冷却的定时器句柄
	FTimerHandle CooldownTimerHandle;

};
