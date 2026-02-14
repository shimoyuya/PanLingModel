// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"


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

	// 创建碰撞盒
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	// 默认关闭碰撞，只有挥砍时才开启
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 设置只检测 Query (重叠和射线)，不参与物理模拟
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// 假设敌人的碰撞体类型是 Pawn
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void APanLingWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// 绑定重叠事件
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APanLingWeapon::OnBoxOverlap);

	// 添加：检查碰撞盒的设置
	UE_LOG(LogTemp, Warning, TEXT("CollisionBox Response to Pawn: %d"),
		CollisionBox->GetCollisionResponseToChannel(ECC_Pawn));
}

// Called every frame
void APanLingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APanLingWeapon::SetCollisionEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APanLingWeapon::ClearHitActors()
{
	HitActors.Empty();
}

void APanLingWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 防止砍到自己
	// 2. 防止砍到重复的目标
	// 3. 确保打到的是有效的 Actor
	UE_LOG(LogTemp, Warning, TEXT("触发重叠"));
	if (OtherActor && OtherActor != GetOwner() && !HitActors.Contains(OtherActor))
	{
		// 加入已击中列表
		HitActors.Add(OtherActor);

		// 打印日志方便调试
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OtherActor->GetName());

		if (!GetInstigatorController())
		{
			UE_LOG(LogTemp, Warning, TEXT("GetInstigatorController() is null"));
		}
		// TODO: 这里可以调用你之前写的 AttributeComponent 的 ApplyHealthChange，或者使用 UE 自带的伤害系统
		// 我们先用 UE 自带的，它通用性更好：
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}
}
