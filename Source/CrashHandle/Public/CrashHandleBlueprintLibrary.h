// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CrashHandleBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CRASHHANDLE_API UCrashHandleBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	static FString ResourceVersion;
public:
	static FString BinaryVersion;

	UFUNCTION(BlueprintCallable, Category = "CrashHandleBlueprintLibrary")
	static void ReportError(const FString& Error);
	UFUNCTION(BlueprintCallable, Category = "CrashHandleBlueprintLibrary")
	static void UpdateResourceVersion(const FString& Version);
	UFUNCTION(BlueprintCallable, Category = "CrashHandleBlueprintLibrary")
	static bool EnableErrorReport();

	UFUNCTION(BlueprintCallable, Category = "CrashHandleBlueprintLibrary")
	static void CrashTest();

	static TSharedPtr<FJsonObject> GetDeviceInfo();
	static void InitVersion(const FString& Version);
};
