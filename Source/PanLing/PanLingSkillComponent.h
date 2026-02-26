// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PanLingSkillBase.h" // 引入技能基类
#include "PanLingSkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UPanLingSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPanLingSkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 在编辑器里配置：角色出生时默认携带的技能列表 (比如槽位1放火球，槽位2放震地)
	UPROPERTY(EditDefaultsOnly, Category = "Skills")
	TArray<TSubclassOf<UPanLingSkillBase>> StartingSkillClasses;

	// 运行时实际实例化的技能对象数组
	// UPROPERTY() 宏极其重要！它能防止这些 UObject 被 UE5 的垃圾回收器(GC)意外清理掉。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skills")
	TArray<UPanLingSkillBase*> InstancedSkills;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 按下按键时，调用对应槽位的技能 (比如传入 0 代表第一个技能)
	UFUNCTION(BlueprintCallable, Category = "Skills")
	void CastSkillAtIndex(int32 SkillIndex);

	// 获取所有技能，以后给 UI 刷新技能栏用
	UFUNCTION(BlueprintCallable, Category = "Skills")
	TArray<UPanLingSkillBase*> GetSkills() const { return InstancedSkills; }
};
