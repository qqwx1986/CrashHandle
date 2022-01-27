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
	UPROPERTY(config, EditAnywhere, Category = "Setting")
	FString ServerUrl;

	UPROPERTY(config, EditAnywhere, Category = "Setting")
	FString ErrorReportPath = TEXT("/receiverError");

	UPROPERTY(config, EditAnywhere, Category = "Setting")
	bool bEnableDev = false;
	
	UPROPERTY(config, EditAnywhere, Category = "Setting")
	FString DevServerUrl;
	
	UPROPERTY(config, EditAnywhere, Category = "Setting")
	bool bGenCrashConfig = true;

	UPROPERTY(config, EditAnywhere, Category = "Setting")
	bool bAndroidCrashReport = false;

	UPROPERTY(config, EditAnywhere, Category = "Setting")
	bool bIOSCrashReport = false;

	UPROPERTY(config, EditAnywhere, Category = "Setting")
	bool bLinuxCrashReport = false;
};