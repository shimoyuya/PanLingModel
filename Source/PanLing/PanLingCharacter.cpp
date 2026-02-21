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
#include "Kismet/KismetMathLibrary.h" // 用于获取看向目标的旋转 (FindLookAtRotation)
#include "PanLingEnemy.h"             // 用于识别扫描到的 Actor 是否是敌人

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

		// 绑定锁定按键 (Triggered 表示按下时触发一次)
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &APanLingCharacter::ToggleLockOn);
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
	// 只能在空闲状态，或者已经在攻击状态(用于连招) 时发起攻击
	if (ActionState == EActionState::HitReact || ActionState == EActionState::Dead) return;

	// 假设你已经创建并初始化了 CombatComp
	if (CombatComp)
	{
		ActionState = EActionState::Attacking;
		CombatComp->RequestAttack();
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
	// 如果不是空闲状态（比如正在攻击或正在受击），禁止移动！
	if (ActionState != EActionState::Unoccupied) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	//当在攻击的时候不能移动
	if (Controller != nullptr )
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

	if (Controller != nullptr && !bIsLockedOn)
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

void APanLingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 如果处于锁定状态并且目标有效
	if (bIsLockedOn && LockedTarget)
	{
		// 检查目标是否跑得太远（超出半径的 20% 就断开锁定）
		float Distance = FVector::Distance(GetActorLocation(), LockedTarget->GetActorLocation());
		if (Distance > LockOnRadius * 1.2f)
		{
			ClearLockOn();
			return;
		}

		// --- 计算相机的目标旋转角度 ---
		FVector CameraLocation = GetFollowCamera()->GetComponentLocation();

		// 我们通常希望看向敌人的胸口而不是脚底(Root位置)，所以 Z 轴可以稍微抬高一点
		FVector TargetLocation = LockedTarget->GetActorLocation() + FVector(0.f, 0.f, 20.f);

		// 寻找从相机看向目标的旋转值
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);

		// 我们通常不希望锁定改变相机的 Roll（翻滚角），并且 Pitch（俯仰角）可以稍微做限制
		TargetRotation.Roll = 0.f;

		// 获取玩家控制器当前的旋转值
		FRotator CurrentRotation = GetController()->GetControlRotation();

		// 使用 RInterpTo 进行平滑插值运算
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, LockOnInterpSpeed);

		// 设置给控制器，这会驱动 SpringArm 移动
		GetController()->SetControlRotation(NewRotation);
	}
}

void APanLingCharacter::ToggleLockOn()
{
	if (bIsLockedOn)
	{
		// 如果已经锁定，则取消锁定
		ClearLockOn();
	}
	else
	{
		// 如果没有锁定，尝试寻找目标
		FindLockOnTarget();
	}
}

void APanLingCharacter::FindLockOnTarget()
{
	// 获取相机的位置和朝向，用来判断敌人在不在屏幕前方
	FVector CameraLocation = GetFollowCamera()->GetComponentLocation();
	FVector CameraForward = GetFollowCamera()->GetForwardVector();

	// 准备球形检测的数据
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 忽略自己
	QueryParams.AddIgnoredActor(CombatComp->GetEquippedWeapon());

	// 在角色周围画一个球型碰撞进行检测 (Sweep)
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn, // 假设敌人是 Pawn 碰撞通道，具体视你的项目而定
		Sphere,
		QueryParams
	);

	AActor* BestTarget = nullptr;
	//记录最短距离。初始值设为我们搜索半径的平方
	// 使用 DistSquared（距离的平方）比 Dist（真实距离）更快，因为它省去了消耗性能的开根号运算 (Sqrt)
	float ClosestDistanceSq = LockOnRadius * LockOnRadius;

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			// 确认扫描到的是敌人 (使用强转或者判断 Class)
			if (HitActor && HitActor->IsA(APanLingEnemy::StaticClass()))
			{
				// 计算从相机到敌人的方向向量
				FVector DirToTarget = (HitActor->GetActorLocation() - CameraLocation).GetSafeNormal();

				// 计算点乘 (Dot Product)
				// CameraForward 和 DirToTarget 都是单位向量，点乘结果在 -1 到 1 之间。
				// 1代表正前方，0代表正侧面，-1代表正后方。
				float Dot = FVector::DotProduct(CameraForward, DirToTarget);

				// 筛选条件：目标必须在镜头前方一定角度内
				// 只要敌人在我们前方一定角度内 (Dot > 0.4 大约是前方 130 度的扇形视野)
				if (Dot > 0.4f)
				{
					// 计算主角到敌人的距离平方
					float DistanceSq = FVector::DistSquared(GetActorLocation(), HitActor->GetActorLocation());

					// 如果这个敌人的距离比我们之前记录的最短距离还要小，就更新最佳目标
					if (DistanceSq < ClosestDistanceSq)
					{
						ClosestDistanceSq = DistanceSq;
						BestTarget = HitActor;
					}
				}
			}
		}
	}

	if (BestTarget)
	{
		LockedTarget = BestTarget;
		bIsLockedOn = true;

		// 【关键】：改变移动模式为“扫射(Strafe)”
		// 关闭“根据移动方向旋转角色”，开启“使用控制器的偏航角”
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;

		// 将 BestTarget 转换为 APanLingEnemy 并显示准星
		if (APanLingEnemy* EnemyTarget = Cast<APanLingEnemy>(LockedTarget))
		{
			EnemyTarget->ShowLockOnUI();
		}
	}
}

void APanLingCharacter::ClearLockOn()
{
	// 在清空锁定目标之前，先隐藏当前目标的准星
	if (LockedTarget)
	{
		if (APanLingEnemy* EnemyTarget = Cast<APanLingEnemy>(LockedTarget))
		{
			EnemyTarget->HideLockOnUI();
		}
	}

	LockedTarget = nullptr;
	bIsLockedOn = false;

	// 恢复为自由视角的移动模式
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}
