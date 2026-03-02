// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingDamageNumberActor.h"
#include "Components/WidgetComponent.h"
#include "PanLingDamageTextWidget.h"

// Sets default values
APanLingDamageNumberActor::APanLingDamageNumberActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsActive = false;

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComp"));
	RootComponent = DamageWidgetComp;

	// 【核心设置】将 UI 设置为屏幕空间 (Screen Space)
	// 这样无论镜头在哪，数字总是面朝玩家，且不会被墙壁遮挡
	DamageWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	// 让组件自动适应内部文字的大小
	DamageWidgetComp->SetDrawAtDesiredSize(true);
}

// Called when the game starts or when spawned
void APanLingDamageNumberActor::BeginPlay()
{
	Super::BeginPlay();

	// 游戏一开始，所有的飘字刚生成时都应该是隐藏、未激活状态
	DeactivateDamageNumber();
}

// Called every frame
void APanLingDamageNumberActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APanLingDamageNumberActor::ShowDamage(float DamageAmount)
{
	if (DamageWidgetComp)
	{
		// 尝试获取组件内的 Widget 实例，并转换为我们写的 C++ 类
		UPanLingDamageTextWidget* DamageWidget = Cast<UPanLingDamageTextWidget>(DamageWidgetComp->GetUserWidgetObject());

		if (DamageWidget)
		{
			// 调用刚才写的函数设置数字
			DamageWidget->SetDamageText(DamageAmount);
		}
	}
}

void APanLingDamageNumberActor::ActivateDamageNumber(FVector Location, float DamageAmount)
{
	bIsActive = true;

	// 1. 移动到指定位置
	SetActorLocation(Location);

	// 2. 取消隐藏，启用更新
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	// 3. 调用蓝图事件去更新 UI 上的文字和播放动画
	ShowDamage(DamageAmount);

	// 4. 设置一个定时器，比如 1.5 秒后自动回收这个飘字（而不是 Destroy！）
	FTimerHandle ReturnTimerHandle;
	GetWorldTimerManager().SetTimer(ReturnTimerHandle, this, &APanLingDamageNumberActor::DeactivateDamageNumber, 1.5f, false);
}

void APanLingDamageNumberActor::DeactivateDamageNumber()
{
	bIsActive = false;

	// 隐藏模型/UI，并关闭更新，彻底节省性能
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}