// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingDamageNumberActor.h"
#include "Components/WidgetComponent.h"
#include "PanLingDamageTextWidget.h"

// Sets default values
APanLingDamageNumberActor::APanLingDamageNumberActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	// 设置寿命：1.5秒后自动销毁自己，防止内存泄漏！
	SetLifeSpan(1.5f);
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

