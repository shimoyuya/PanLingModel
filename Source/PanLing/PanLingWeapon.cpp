// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h" //


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
	BaseDamage = 20.0f;
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
	if (!bIsTracing || !MeshComp) return;

	// 获取我们在蓝图中要在刀剑模型上添加的两个插槽位置：刀根(Start) 和 刀尖(End)
	FVector TraceStartLoc = MeshComp->GetSocketLocation(FName("TraceStart"));
	FVector TraceEndLoc = MeshComp->GetSocketLocation(FName("TraceEnd"));

	// 配置球形射线：半径20代表刀刃的厚度和判定范围
	FCollisionShape Sphere = FCollisionShape::MakeSphere(20.0f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner()); // 绝不能砍到自己

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
				UGameplayStatics::ApplyDamage(HitActor, BaseDamage, GetInstigatorController(), this, UDamageType::StaticClass());

				//播放粒子特效 (在具体的击中点 ImpactPoint)
				if (HitVFX)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				}

				// 播放音效
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, Hit.ImpactPoint);
				}

				UE_LOG(LogTemp, Warning, TEXT("精准砍中: %s"), *HitActor->GetName());
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

