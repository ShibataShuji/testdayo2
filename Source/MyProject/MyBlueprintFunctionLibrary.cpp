// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

bool UMyBlueprintFunctionLibrary::RunMyPython(int32& ProcessId, FString FullPathOfProgramToRun,
	TArray<FString> CommandlineArgs, bool Hidden)
{
	/*
	コマンドライン引数の配列を空白で結合する
	*/
	FString Args = "";
	if (CommandlineArgs.Num() > 1)
	{
		Args = CommandlineArgs[0];
		for (int32 v = 1; v < CommandlineArgs.Num(); v++)
		{
			Args += " " + CommandlineArgs[v];
		}
	}
	else if (CommandlineArgs.Num() > 0)
	{
		Args = CommandlineArgs[0];
	}


	/*
	新しいプロセスを作成する（CreateProcess関数と似てるもの）
	*/
	uint32 NeedBPUINT32 = 0;
	FProcHandle ProcHandle = FPlatformProcess::CreateProc(
		*FullPathOfProgramToRun,
		*Args,
		Hidden, //trueにしたら、コマンドプロンプトのウィンドウを非表示にする
		false,
		false,
		&NeedBPUINT32,
		0,
		nullptr,
		nullptr);

	ProcessId = NeedBPUINT32;

	if (ProcHandle.IsValid()) {
		/*
		上記のプロセスが終了したまで、待ちます。
		*/
		while (FPlatformProcess::IsProcRunning(ProcHandle));
		FPlatformProcess::CloseProc(ProcHandle);
	}

	return true; //プロセスが終了して、trueを戻る。
}
