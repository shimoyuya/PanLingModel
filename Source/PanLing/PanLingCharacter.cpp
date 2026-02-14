// Copyright Epic Games, Inc. All Rights Reserved.

#include "PanLingCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AttributeComponent.h"
#include "GameplayInterface.h"
#include "DrawDebugHelpers.h" // 用于画调试线，正式发布可去掉
#include "PanLingWeapon.h"
#include "CombatComponent.h"
#include "PanLingPlayerHUD.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APanLingCharacter

APanLingCharacter::APanLingCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// 确保胶囊体可以阻挡武器的检测（对应你武器设置的 ECC_Pawn）
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 创建属性组件
	AttributeComp = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComp"));

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	WeaponSocketName = TEXT("WeaponSocket");
}

//////////////////////////////////////////////////////////////////////////
// Input

void APanLingCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APanLingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APanLingCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APanLingCharacter::Look);

		//交互
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APanLingCharacter::PrimaryInteract);
		//攻击
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APanLingCharacter::Attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APanLingCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 生成武器并交给 CombatComp 装备
	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APanLingWeapon* NewWeapon = GetWorld()->SpawnActor<APanLingWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (NewWeapon && CombatComp)
		{
			CombatComp->EquipWeapon(NewWeapon);
		}
	}
	// 创建并显示 HUD
	// 确保这是本地玩家控制的角色（服务器上的 AI 角色不需要显示屏幕 UI）
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		// 创建 Widget
		PlayerHUDInstance = CreateWidget<UPanLingPlayerHUD>(PlayerController, PlayerHUDClass);

		if (PlayerHUDInstance)
		{
			// 添加到屏幕上
			PlayerHUDInstance->AddToViewport();

			// 把我们身上的属性组件传给 HUD，让它开始监听血量
			PlayerHUDInstance->InitializeHUD(AttributeComp);
		}
	}
}

void APanLingCharacter::Attack()
{
	// 检查是否可以攻击（如果没有在攻击中）
	if (CombatComp && !CombatComp->IsAttacking() && AttackMontage)
	{
		CombatComp->SetAttacking(true);
		PlayAnimMontage(AttackMontage);
	}
}

APanLingWeapon* APanLingCharacter::GetEquippedWeapon() const
{
	return nullptr;
}

void APanLingCharacter::AttackEnd()
{

}

float APanLingCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComp)
	{
		// 扣血（传入负数的伤害值）
		AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);

		// 可选：打印日志看自己被扣了多少血
		UE_LOG(LogTemp, Warning, TEXT("Player Took Damage! Current Health: %f"), AttributeComp->GetHealth()); // 注意这里最好写个 GetHealth() 函数，或者直接看日志
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void APanLingCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APanLingCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APanLingCharacter::PrimaryInteract()
{
	// 获取视线起点 (摄像机位置)
	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);

	// 视线终点 (向前 500 单位)
	FVector End = Location + (Rotation.Vector() * 500.0f);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 我们假设宝箱是 WorldDynamic

	FHitResult Hit;
	// 发射射线 (Line Trace)
	// 如果你想检测更宽的范围，可以用 SphereTraceSingle
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Location, End, ObjectQueryParams);

	// 绘制调试线 (绿色表示击中，红色表示未击中)
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Location, End, LineColor, false, 2.0f, 0, 2.0f);

	if (bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			// 核心逻辑：检查是否实现了接口
			if (HitActor->Implements<UGameplayInterface>())
			{
				// 调用接口函数
				// 注意语法：IGameplayInterface::Execute_函数名(对象, 参数...)
				IGameplayInterface::Execute_Interact(HitActor, this);
				UE_LOG(LogTemp, Warning, TEXT("检测到"));
			}
		}
	}
}
