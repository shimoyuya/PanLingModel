// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h" 
#include "NiagaraFunctionLibrary.h"
#include "WeaponDataAsset.h"
#include "GameFramework/PlayerController.h" // 播放震动需要用到玩家控制器
#include "Camera/CameraShakeBase.h"         // 震动类基类
#include "PanLingDamageNumberActor.h"
#include "AttributeComponent.h"

// Sets default values
APanLingWeapon::APanLingWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// 默认设置：武器不应该阻挡角色的移动 (Collision Profile)
	// 这里设为 NoCollision，攻击判定我们后面用专门的 Trace 或者 BoxComponent 做
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));

	bIsTracing = false;
}

void APanLingWeapon::InitializeWeapon(UWeaponDataAsset* InWeaponData)
{
	WeaponData = InWeaponData;

	// 如果有数据且有模型，立马更新武器的外观！
	if (WeaponData && WeaponData->WeaponMesh && MeshComp)
	{
		MeshComp->SetStaticMesh(WeaponData->WeaponMesh);
	}
}

// Called when the game starts or when spawned
void APanLingWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APanLingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APanLingWeapon::StartWeaponTrace()
{
	bIsTracing = true;
	ClearHitActors(); // 每次全新挥砍时，清空击中列表
}

void APanLingWeapon::DoWeaponTrace()
{
	if (!bIsTracing || !MeshComp || !WeaponData) return; // 确保WeaponData存在

	// 获取我们在蓝图中要在刀剑模型上添加的两个插槽位置：刀根(Start) 和 刀尖(End)
	FVector TraceStartLoc = MeshComp->GetSocketLocation(FName("TraceStart"));
	FVector TraceEndLoc = MeshComp->GetSocketLocation(FName("TraceEnd"));

	// 配置球形射线：半径20代表刀刃的厚度和判定范围
	FCollisionShape Sphere = FCollisionShape::MakeSphere(20.0f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner()); // 绝不能砍到自己

	// 【新增核心逻辑】：动态计算实际伤害
	// 1. 先用武器的基础伤害作为保底默认值
	float ActualDamage = WeaponData->BaseDamage;

	// 2. 尝试获取武器主人的属性组件
	AActor* WeaponOwner = GetOwner();
	if (WeaponOwner)
	{
		// 使用 FindComponentByClass 可以完美解耦！不管拿武器的是玩家还是未来的高级AI，只要有这个组件就能算伤害
		UAttributeComponent* OwnerAttributeComp = WeaponOwner->FindComponentByClass<UAttributeComponent>();
		if (OwnerAttributeComp)
		{
			// 3. 用主人的动态面板攻击力，替换掉武器的静态基础伤害
			ActualDamage = OwnerAttributeComp->GetAttackPower();
		}
	}

	TArray<FHitResult> HitResults;

	// 从刀根向刀尖扫出一个胶囊体范围
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		TraceStartLoc,
		TraceEndLoc,
		FQuat::Identity,
		ECC_Pawn, // 依然假设敌人是 Pawn 通道
		Sphere,
		QueryParams
	);

	// 【酷炫功能】画出武器挥舞的轨迹，方便你在编辑器里调试判定范围 (正式打包时可去掉)
	DrawDebugCapsule(GetWorld(), (TraceStartLoc + TraceEndLoc) * 0.5f, FVector::Distance(TraceStartLoc, TraceEndLoc) * 0.5f, 20.0f, FQuat::FindBetweenVectors(FVector::UpVector, TraceEndLoc - TraceStartLoc), FColor::Red, false, 2.0f);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			// 如果打到了有效敌人，并且这一刀还没打过他
			if (HitActor && !HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);

				// 造成伤害
				UGameplayStatics::ApplyDamage(HitActor, ActualDamage, GetInstigatorController(), this, UDamageType::StaticClass());

				//播放粒子特效 (在具体的击中点 ImpactPoint)
				if (WeaponData->HitVFX)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponData->HitVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				}

				// 播放音效
				if (WeaponData->HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, WeaponData->HitSound, Hit.ImpactPoint);
				}

				UE_LOG(LogTemp, Warning, TEXT("精准砍中: %s"), *HitActor->GetName());

				// --- 顿帧与屏幕震动 ---
				// 1. 播放屏幕震动
				if (WeaponData->HitCameraShake)
				{
					// 获取攻击者的玩家控制器
					APlayerController* PC = Cast<APlayerController>(GetInstigatorController());
					if (PC)
					{
						// 在客户端播放震动
						PC->ClientStartCameraShake(WeaponData->HitCameraShake);
					}
				}
				// 2. 触发顿帧 (Hit Stop)
				if (WeaponData->HitStopDuration > 0.0f)
				{
					// 设置全局时间膨胀（让游戏变慢）
					UGameplayStatics::SetGlobalTimeDilation(GetWorld(), WeaponData->HitStopTimeDilation);

					// 【面试考点/干货】：因为游戏全局时间变慢了，所以 UE 内置的 Timer 也会跟着变慢。
					// 想要在现实世界的 0.05 秒后恢复，定时器的 Delay 需要乘以当前的膨胀系数。
					// 比如：现实要等 0.05 秒，游戏流速是 0.1，那么游戏内只需要过去 0.005 秒。
					float DilatedDelay = WeaponData->HitStopDuration * WeaponData->HitStopTimeDilation;

					// 设定定时器，时间到后调用 ResetHitStop
					GetWorldTimerManager().SetTimer(TimerHandle_HitStop, this, &APanLingWeapon::ResetHitStop, DilatedDelay, false);
				}
			}
		}
	}
}

void APanLingWeapon::StopWeaponTrace()
{
	bIsTracing = false;
}

void APanLingWeapon::ClearHitActors()
{
	HitActors.Empty();
}

void APanLingWeapon::ResetHitStop()
{
	// 恢复全局时间流速为正常的 1.0f
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}
