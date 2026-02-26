// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Target.generated.h"

/**
 * 自定义 EQS 上下文：从 AI 的黑板中获取名为 "TargetActor" 的对象
 * 这样 EQS 生成器就可以围绕这个目标生成点。
 */
UCLASS()
class PANLING_API UEnvQueryContext_Target : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	// 虚幻引擎 EQS 系统的核心接口，用于返回我们需要的上下文数据
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};
