// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "PanLingCharacter.h"
#include "PanLingWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bIsAttacking = false;
	bSaveAttack = false;
	ComboCount = 0;
	CurrentTarget = nullptr;
	bIsRotating = false;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 初始化时获取拥有者
	CharacterOwner = Cast<ACharacter>(GetOwner());
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 如果正在攻击且找到了目标，执行平滑转身
	if (bIsRotating && CurrentTarget && GetOwner())
	{
		
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
		if (OwnerCharacter)
		{
			// 计算从玩家指向敌人的方向向量
			FVector DirectionToTarget = CurrentTarget->GetActorLocation() - OwnerCharacter->GetActorLocation();
			DirectionToTarget.Z = 0.f; // 忽略Z轴（高度）差，防止角色抬头或低头

			// 获取目标旋转值
			FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(DirectionToTarget);

			// 使用 RInterpTo 平滑插值
			FRotator NewRotation = FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
			OwnerCharacter->SetActorRotation(NewRotation);
		}
	}
}

void UCombatComponent::EquipWeapon(APanLingWeapon* WeaponToEquip)
{
	if (WeaponToEquip && CharacterOwner)
	{
		EquippedWeapon = WeaponToEquip;

		// 附加到角色的武器插槽
		// 这里的 "WeaponSocket" 可以提取为变量，为了简单先硬编码或从 Character 获取
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedWeapon->AttachToComponent(CharacterOwner->GetMesh(), AttachRules, FName("WeaponSocket"));

		// 可以在这里设置武器的拥有者，方便后续伤害计算
		EquippedWeapon->SetOwner(CharacterOwner);
		EquippedWeapon->SetInstigator(CharacterOwner);
	}
}

APanLingWeapon* UCombatComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

void UCombatComponent::StartWeaponTrace()
{
	if (EquippedWeapon)
	{
		// 开启武器的碰撞检测
		EquippedWeapon->SetCollisionEnabled(true);
		UE_LOG(LogTemp, Warning, TEXT("Weapon collision enabled"));
	}
}

void UCombatComponent::StopWeaponTrace()
{
	if (EquippedWeapon)
	{
		// 关闭武器的碰撞检测，并清空已经击中过的目标列表（防止一次挥砍重复击中同一个人）
		EquippedWeapon->SetCollisionEnabled(false);
		EquippedWeapon->ClearHitActors();
	}
}

void UCombatComponent::RequestAttack()
{
	if (!GetOwner()) { return; }

	if (bIsAttacking)
	{
		// 如果已经在攻击了，说明玩家在狂按。我们记录下这次按键（输入缓存）
		bSaveAttack = true;
	}
	else
	{
		// 如果没有在攻击，则是起手第一招
		bIsAttacking = true;
		ComboCount = 1;
		PerformAttack();
	}
}

// 这个函数稍后会由 C++ 的 AnimNotify 调用
void UCombatComponent::CheckCombo()
{
	if (bSaveAttack)
	{
		// 如果玩家在连招窗口期内按过攻击键
		bSaveAttack = false; // 清空缓存
		ComboCount++;        // 段数+1

		// 假设我们最多只有 3 段连招
		if (ComboCount > 2)
		{
			ComboCount = 1; // 循环连招或者你可以调用 ResetCombatState() 强制结束
		}

		PerformAttack();     // 播放下一段动画
	}
	else
	{
		// 如果玩家没有按键，说明不想连招了，什么也不做，让动画自然结束即可
	}
}

void UCombatComponent::ResetCombatState()
{
	bIsAttacking = false;
	bSaveAttack = false;
	ComboCount = 0;

	// 攻击结束，关闭旋转
	bIsRotating = false;
	CurrentTarget = nullptr;

	CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UCombatComponent::PerformAttack()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && ComboMontage)
	{
		// 在攻击开始时
		CharacterOwner->GetCharacterMovement()->bOrientRotationToMovement = false;
		// 每次发起新的攻击动作时，寻找最近的敌人
		FindNearestTarget();

		// 动态生成 Montage Section 的名字，比如 "Attack1", "Attack2"
		FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), ComboCount));

		// 播放蒙太奇，并跳转到对应的 Section
		OwnerCharacter->PlayAnimMontage(ComboMontage, 1.0f, SectionName);
	}
}

void UCombatComponent::FindNearestTarget()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	UWorld* World = GetWorld();
	if (!World) return;

	CurrentTarget = nullptr;
	bIsRotating = false;

	FVector PlayerLoc = OwnerChar->GetActorLocation();
	FVector PlayerForward = OwnerChar->GetActorForwardVector();

	// 配置碰撞检测球体
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TargetingRadius);
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerChar); // 忽略自己
	if (EquippedWeapon)
	{
		QueryParams.AddIgnoredActor(EquippedWeapon);
	}

	// 执行场景检测 (假设敌人属于 Pawn 碰撞通道)
	bool bHit = World->SweepMultiByChannel(HitResults, PlayerLoc, PlayerLoc, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	if (bHit)
	{
		float ClosestDistanceSq = MAX_flt;

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			// 这里建议加上你的接口判断，比如 if(HitActor->Implements<UGameplayInterface>())，或者判断是否是 APanLingEnemy
			if (HitActor)
			{
				FVector DirToTarget = (HitActor->GetActorLocation() - PlayerLoc).GetSafeNormal();

				// 【核心亮点】使用点乘判断敌人是否在前方。
				// 点乘结果 > 0 说明在前方90度内。我们用 cos(TargetingAngle/2) 算出阈值
				float DotProduct = FVector::DotProduct(PlayerForward, DirToTarget);
				float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(TargetingAngle / 2.0f));

				if (DotProduct > AngleThreshold)
				{
					// 使用距离平方比较，性能比计算真实距离(开根号)更好
					float DistanceSq = FVector::DistSquared(PlayerLoc, HitActor->GetActorLocation());
					if (DistanceSq < ClosestDistanceSq)
					{
						ClosestDistanceSq = DistanceSq;
						CurrentTarget = HitActor;
					}
				}
			}
		}
	}

	// 如果找到了目标，开启 Tick 中的旋转逻辑
	if (CurrentTarget)
	{
		bIsRotating = true;
	}
}

void UCombatComponent::AttackDash(float DashForce)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar)
	{
		// 沿着角色当前的正前方施加推力
		FVector DashVelocity = OwnerChar->GetActorForwardVector() * DashForce;

		// LaunchCharacter 会在物理层面推开角色。参数为 (速度向量, 是否覆盖XY轴速度, 是否覆盖Z轴速度)
		OwnerChar->LaunchCharacter(DashVelocity, true, false);
	}
}