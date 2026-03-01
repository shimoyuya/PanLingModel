// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class UPanLingEffectBase;

// 定义修改器的类型：加法 或 乘法
UENUM(BlueprintType)
enum class EPanLingModType : uint8
{
	Add         UMETA(DisplayName = "Addition"),
	Multiply    UMETA(DisplayName = "Multiplication")
};

// 定义单个修改器的数据结构
USTRUCT(BlueprintType)
struct FPanLingModifier
{
	GENERATED_BODY()

	// 修改器的数值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;

	// 修改器的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPanLingModType ModType = EPanLingModType::Add;

	// 修改器的来源ID（比如 "Weapon_IronSword" 或 "Buff_Poison"），用于后续精准移除
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SourceID;

	// 默认构造
	FPanLingModifier() {}
	FPanLingModifier(float InValue, EPanLingModType InType, FName InSource)
		: Value(InValue), ModType(InType), SourceID(InSource) {
	}
};

// 定义完整的属性数据结构
USTRUCT(BlueprintType)
struct FPanLingAttributeData
{
	GENERATED_BODY()

	// 基础值（裸体无装备无Buff时的数值）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	float BaseValue = 0.f;

	// 当前最终值（经过所有修改器计算后的数值）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	float CurrentValue = 0.f;

	// 当前挂载在该属性上的所有修改器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TArray<FPanLingModifier> Modifiers;

	// 核心函数：当修改器发生变化时，重新计算当前值
	void Recalculate()
	{
		float Additive = 0.f;
		float Multiplicative = 1.f; // 乘法基础为1

		for (const FPanLingModifier& Mod : Modifiers)
		{
			if (Mod.ModType == EPanLingModType::Add)
			{
				Additive += Mod.Value;
			}
			else if (Mod.ModType == EPanLingModType::Multiply)
			{
				Multiplicative += Mod.Value;
			}
		}

		// 最终公式：(基础值 + 附加值) * (1 + 乘法加成百分比)
		CurrentValue = (BaseValue + Additive) * Multiplicative;
	}
};


//血量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, class UAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
// 体力值改变委托，用于更新 UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStaminaChanged, AActor*, InstigatorActor, UAttributeComponent*, OwningComp, float, NewStamina, float, Delta);
//等级
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32, NewLevel, int32, MaxLevel);
//经验值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEXPChanged, float, CurrentEXP, float, MaxEXP);
// 声明一个通用的属性变化委托：参数1是属性名(如 "AttackPower")，参数2是新的属性值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeValueChanged, FName, AttributeName, float, NewValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

	// -- 新的属性系统声明 --
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    FPanLingAttributeData MaxHealthData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    FPanLingAttributeData AttackPowerData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    FPanLingAttributeData DefenseData;

    // -- 核心方法声明 --

    // 给指定属性添加修改器 (传入枚举或字符串来区分修改哪个属性)
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    void AddModifierToAttribute(FName AttributeName, FPanLingModifier NewModifier);

    // 移除指定来源的所有修改器（例如卸下武器时调用）
    UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
    void RemoveModifierFromAttribute(FName AttributeName, FName SourceID);

	/* 通用属性函数 */
	// 属性变化事件
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnAttributeValueChanged OnAttributeValueChanged;

	// 添加 Getter 函数，方便 UI 初始化时获取当前值
	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetAttackPower() const { return AttackPowerData.CurrentValue; }

	UFUNCTION(BlueprintPure, Category = "Attributes")
	float GetDefense() const { return DefenseData.CurrentValue; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 当前生命值（注意：当前血量不需要修改器，它受MaxHealth上限控制）
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    float CurrentHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** * 应用血量变化
	 * @param Delta: 变化量 (负数表示受到伤害，正数表示治疗)
	 * @return: 实际变化了多少 (可能被 Clamp 截断)
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	/** 获取当前是否存活 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const { return CurrentHealth > 0.0f; }

	/** 获取最大生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const { return MaxHealthData.CurrentValue; }
	/** 获取当前生命值 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const { return CurrentHealth; }

	/** 广播血量变化的事件 */
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChanged OnHealthChanged;

/*
	体力值
*/
public:
	/** 广播体力值变化的事件 */
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;

	// 消耗精力的接口
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyStaminaChange(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxStamina() const { return MaxStamina; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Stamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxStamina = 100.0f;

	// 精力恢复速度 (每秒恢复多少)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float StaminaRegenRate = 15.0f;

	// 控制是否暂停恢复 (比如刚闪避完/攻击完，停顿1秒再恢复)
	bool bCanRegenStamina;
	FTimerHandle TimerHandle_StaminaRegenDelay;

	// --- 养成系统 (Progression) ---
public:
	// 添加经验值的对外接口
	UFUNCTION(BlueprintCallable, Category = "Attributes|Progression")
	void AddEXP(float EXPAmount);

	// 委托：当经验值变化时广播
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnEXPChanged OnEXPChanged;

	// 委托：当等级变化时广播
	UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
	FOnLevelChanged OnLevelChanged;

protected:
	// 当前等级
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 Level = 1;

	// 最大等级上限
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	int32 MaxLevel = 100;

	// 当前经验值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float CurrentEXP = 0.0f;

	// 升级所需的经验值上限 (初始值)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Progression")
	float MaxEXP = 100.0f;

	/* Buff系统 */
protected:
	// 使用 UPROPERTY 标记数组，保证实例化的 UObject 不会被 UE 的垃圾回收（GC）清理掉
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PanLing|Effects")
	TArray<UPanLingEffectBase*> ActiveEffects;

public:
	// 给角色应用一个新的 Buff
	UFUNCTION(BlueprintCallable, Category = "PanLing|Effects")
	void ApplyEffect(TSubclassOf<UPanLingEffectBase> EffectClass);

	// 手动移除一个特定的 Buff（如：吃解药移除中毒状态）
	UFUNCTION(BlueprintCallable, Category = "PanLing|Effects")
	void RemoveEffect(UPanLingEffectBase* EffectToRemove);


};
