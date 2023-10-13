// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyInterface.h"
#include "Async/AsyncWork.h"
#include "MyBlueprintFunctionLibrary.generated.h"



class MultithreadedTask : public FNonAbandonableTask {
public:
	UObject* object;

	MultithreadedTask(UObject* object) { this->object = object; }
	// ワーク実行
	// 非同期処理を指定された関数の実行
	void DoWork()
	{
		IMyInterface::Execute_MultithreadFunction(object);
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(MultithreadedTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MyInterface")
	// 非同期処理を呼び関数
	static void CallMultithreadFunction(UObject* object) {
		// 非同期処理のタスクをバックグラウンドで実行
		(new FAutoDeleteAsyncTask<MultithreadedTask>(object))->StartBackgroundTask();
	}


	UFUNCTION(BlueprintCallable, Category = "RunPython")
	static bool RunMyPython(int32& ProcessId,
		FString FullPathOfProgramToRun, TArray<FString> CommandlineArgs, bool Hidden);

};

// このMultithreadedTaskクラスはタスクを定義するクラスです。
