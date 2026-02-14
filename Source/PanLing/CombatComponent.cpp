// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "PanLingCharacter.h"
#include "PanLingWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bIsAttacking = false;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 初始化时获取拥有者
	CharacterOwner = Cast<ACharacter>(GetOwner());
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::EquipWeapon(APanLingWeapon* WeaponToEquip)
{
	if (WeaponToEquip && CharacterOwner)
	{
		EquippedWeapon = WeaponToEquip;

		// 附加到角色的武器插槽
		// 这里的 "WeaponSocket" 可以提取为变量，为了简单先硬编码或从 Character 获取
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedWeapon->AttachToComponent(CharacterOwner->GetMesh(), AttachRules, FName("WeaponSocket"));

		// 可以在这里设置武器的拥有者，方便后续伤害计算
		EquippedWeapon->SetOwner(CharacterOwner);
		EquippedWeapon->SetInstigator(CharacterOwner);
	}
}

APanLingWeapon* UCombatComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

void UCombatComponent::StartWeaponTrace()
{
	if (EquippedWeapon)
	{
		// 开启武器的碰撞检测
		EquippedWeapon->SetCollisionEnabled(true);
		UE_LOG(LogTemp, Warning, TEXT("Weapon collision enabled"));
	}
}

void UCombatComponent::StopWeaponTrace()
{
	if (EquippedWeapon)
	{
		// 关闭武器的碰撞检测，并清空已经击中过的目标列表（防止一次挥砍重复击中同一个人）
		EquippedWeapon->SetCollisionEnabled(false);
		EquippedWeapon->ClearHitActors();
	}
}
