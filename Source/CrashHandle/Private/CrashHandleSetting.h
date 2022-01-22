// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CrashHandleSetting.generated.h"

/**
 * 
 */
UCLASS(config = CrashHandle, defaultconfig, meta = (DisplayName = "CrashHandle"))
class UCrashHandleSetting : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere, Category = "Setting", meta = (DisplayName = "Server Url"))
	FString ServerUrl;
};
