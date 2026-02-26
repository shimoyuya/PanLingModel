// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingSkillComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UPanLingSkillComponent::UPanLingSkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPanLingSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// 游戏开始时，根据配置的类，生成实际的技能对象
	for (TSubclassOf<UPanLingSkillBase> SkillClass : StartingSkillClasses)
	{
		if (SkillClass)
		{
			// 使用 NewObject 创建 UObject 实例，将组件自身(this)作为其 Outer
			UPanLingSkillBase* NewSkill = NewObject<UPanLingSkillBase>(this, SkillClass);
			if (NewSkill)
			{
				InstancedSkills.Add(NewSkill);
			}
		}
	}
	
}


// Called every frame
void UPanLingSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPanLingSkillComponent::CastSkillAtIndex(int32 SkillIndex)
{
	// 数组越界保护
	if (InstancedSkills.IsValidIndex(SkillIndex))
	{
		UPanLingSkillBase* SkillToCast = InstancedSkills[SkillIndex];
		ACharacter* MyOwner = Cast<ACharacter>(GetOwner());

		if (SkillToCast && MyOwner)
		{
			// 如果技能可用，就会在这个函数内部扣精力并播放动画
			SkillToCast->CastSkill(MyOwner);
		}
	}
}

