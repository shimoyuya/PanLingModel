// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PanLingEnemy.generated.h"

class UAttributeComponent;
class UCombatComponent;
class APanLingWeapon;
class UAnimMontage;

UCLASS()
class PANLING_API APanLingEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APanLingEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 敌人的属性组件（血量）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComp;

	// 监听血量变化的函数
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UAttributeComponent* OwningComp, float NewHealth, float Delta);

	// 战斗组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCombatComponent* CombatComp;

	// 敌人默认装备的武器类
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<APanLingWeapon> WeaponClass;

	// 敌人的攻击动画
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 重写 UE 内置的受击函数，这是用来接收你在武器里调用的 ApplyDamage 的！
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 供 AI 调用的攻击函数
	void Attack();
};
