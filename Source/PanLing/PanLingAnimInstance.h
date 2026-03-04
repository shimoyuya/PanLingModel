// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PanLingAnimInstance.generated.h"

/**
 * 通用动画实例基类，适用于玩家和敌人
 */
UCLASS()
class PANLING_API UPanLingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 动画初始化时调用（类似 BeginPlay）
	virtual void NativeInitializeAnimation() override;

	// 每帧更新动画时调用（类似 Tick）
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 缓存的角色指针，避免每帧去获取
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class ACharacter* OwnerCharacter;

	// 缓存的移动组件指针
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class UCharacterMovementComponent* CharacterMovement;

	// 当前水平移动速度
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;

	// 是否处于空中（跳跃或掉落）
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	// 是否有移动输入（用于区分是被推开还是主动移动）
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;
};
