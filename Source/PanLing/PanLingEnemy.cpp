// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingEnemy.h"
#include "AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponent.h"
#include "PanLingWeapon.h"
#include "Components/WidgetComponent.h"
#include "PanLingAIController.h"
#include "BrainComponent.h"

// Sets default values
APanLingEnemy::APanLingEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 1. 添加属性组件
	AttributeComp = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComp"));

	// 2. 调整一些基础设置，让敌人看起来更像 AI
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// 确保胶囊体可以阻挡武器的检测（对应你武器设置的 ECC_Pawn）
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// 创建战斗组件
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	// 1. 创建 Widget 组件
	LockOnWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComp"));

	// 2. 附加到敌人的 RootComponent 或 Mesh 上
	// 建议附加在 Mesh 上，并可以稍微往上抬一点，这样准星会在胸口/头部位置
	LockOnWidgetComp->SetupAttachment(GetMesh());
	LockOnWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // 根据你的模型调整高度

	// 3. 【关键】设置为屏幕空间 (Screen Space)，这样UI会永远面朝屏幕并且不受透视缩放影响
	LockOnWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	// 4. 设置默认大小
	LockOnWidgetComp->SetDrawSize(FVector2D(50.f, 50.f));

	// 5. 默认状态下隐藏准星
	LockOnWidgetComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void APanLingEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 绑定血量变化的委托（当血量改变时，自动调用 OnHealthChanged）
	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &APanLingEnemy::OnHealthChanged);
	}

	// 生成并装备武器
	if (WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("生成武器"));
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APanLingWeapon* NewWeapon = GetWorld()->SpawnActor<APanLingWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (NewWeapon && CombatComp)
		{
			CombatComp->EquipWeapon(NewWeapon);
			UE_LOG(LogTemp, Warning, TEXT("装备武器"));
		}
	}
}


// Called every frame
void APanLingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APanLingEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float APanLingEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 拦截引擎自带的伤害，将其转化为我们的 AttributeComp 扣血逻辑
	// 注意传入的 Delta 是负数，所以要加负号：-DamageAmount
	//if (AttributeComp)
	//{
	//	AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	//}

	//// 返回实际造成的伤害（给引擎底层用的）
	//return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 假设你有一个方法获取当前血量，并且敌人还没死
	if (ActualDamage > 0.0f && AttributeComp->IsAlive())
	{
		AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);

		// 1. 播放受击动画 (它会自动打断当前正在播放的攻击动画)
		if (HitReactMontage)
		{
			PlayAnimMontage(HitReactMontage);
		}

		// 2. 停止 AI 行为树一小段时间 (打断它的攻击逻辑)
		if (APanLingAIController* AIController = Cast<APanLingAIController>(GetController()))
		{
			// 获取大脑组件并暂停逻辑，"Hit" 是暂停的原因标签
			if (AIController->GetBrainComponent())
			{
				AIController->GetBrainComponent()->PauseLogic(TEXT("Hit"));

				// 设置一个定时器，在受击动画播放完毕后恢复 AI 逻辑 (假设受击动画长 0.8 秒)
				FTimerHandle TimerHandle_HitReact;
				GetWorldTimerManager().SetTimer(TimerHandle_HitReact, [AIController]()
					{
						if (AIController && AIController->GetBrainComponent())
						{
							AIController->GetBrainComponent()->ResumeLogic(TEXT("Hit"));
						}
					}, 0.8f, false); // 这里的 0.8f 建议替换为 HitReactMontage 的实际长度
			}
		}
	}

	return ActualDamage;

}

void APanLingEnemy::OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// 如果 Delta 是负数，说明是受伤
	if (Delta < 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Took Damage! Current Health: %f"), NewHealth);

		// 检查是否死亡
		if (!AttributeComp->IsAlive())
		{
			UE_LOG(LogTemp, Error, TEXT("Enemy DIED!"));

			// 死亡逻辑：关闭碰撞，停止移动，甚至可以直接销毁或者播放死亡动画
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// 设置寿命，5秒后自动销毁尸体
			SetLifeSpan(5.0f);
		}
	}
}

void APanLingEnemy::Attack()
{
	// 如果不在攻击状态，且有攻击动画，则播放
	if (CombatComp && !CombatComp->IsAttacking() && AttackMontage)
	{
		CombatComp->SetAttacking(true);
		PlayAnimMontage(AttackMontage);

		// 简单处理：使用 Timer 在动画结束后重置攻击状态（假设动画长 1.5 秒）
		// 进阶做法是在蒙太奇里用 AnimNotify 来重置
		FTimerHandle TimerHandle_AttackReset;
		GetWorldTimerManager().SetTimer(TimerHandle_AttackReset, [this]() {
			if (CombatComp) CombatComp->SetAttacking(false);
			}, 1.0f, false);
	}
}

void APanLingEnemy::ShowLockOnUI()
{
	if (LockOnWidgetComp)
	{
		LockOnWidgetComp->SetVisibility(true);
	}
}

void APanLingEnemy::HideLockOnUI()
{
	if (LockOnWidgetComp)
	{
		LockOnWidgetComp->SetVisibility(false);
	}
}
