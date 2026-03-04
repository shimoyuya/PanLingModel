// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPanLingAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 尝试获取拥有这个动画蓝图的 Pawn，并转换为 ACharacter
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	if (OwnerCharacter)
	{
		// 缓存移动组件
		CharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UPanLingAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 如果拥有者为空，说明还没初始化好，直接返回
	if (OwnerCharacter == nullptr || CharacterMovement == nullptr)
	{
		return;
	}

	// 1. 计算水平速度 (忽略 Z 轴上下掉落的速度)
	FVector Velocity = OwnerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	// === 新增：计算方向 ===
	// 获取角色当前的朝向
	FRotator CharacterRotation = OwnerCharacter->GetActorRotation();
	// 计算速度向量相对于角色朝向的角度差 (-180 到 180)
	Direction = CalculateDirection(Velocity, CharacterRotation);

	// 2. 检查是否在空中
	bIsFalling = CharacterMovement->IsFalling();

	// 3. 检查是否有移动意图 (速度大于一定值，且有加速度输入)
	bShouldMove = (Speed > 3.f) && (CharacterMovement->GetCurrentAcceleration().Size() > 0.f);
}
