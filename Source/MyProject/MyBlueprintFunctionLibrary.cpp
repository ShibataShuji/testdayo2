// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

bool UMyBlueprintFunctionLibrary::RunMyPython(int32& ProcessId, FString FullPathOfProgramToRun,
	TArray<FString> CommandlineArgs, bool Hidden)
{
	/*
	�R�}���h���C�������̔z����󔒂Ō�������
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
	�V�����v���Z�X���쐬����iCreateProcess�֐��Ǝ��Ă���́j
	*/
	uint32 NeedBPUINT32 = 0;
	FProcHandle ProcHandle = FPlatformProcess::CreateProc(
		*FullPathOfProgramToRun,
		*Args,
		Hidden, //true�ɂ�����A�R�}���h�v�����v�g�̃E�B���h�E���\���ɂ���
		false,
		false,
		&NeedBPUINT32,
		0,
		nullptr,
		nullptr);

	ProcessId = NeedBPUINT32;

	if (ProcHandle.IsValid()) {
		/*
		��L�̃v���Z�X���I�������܂ŁA�҂��܂��B
		*/
		while (FPlatformProcess::IsProcRunning(ProcHandle));
		FPlatformProcess::CloseProc(ProcHandle);
	}

	return true; //�v���Z�X���I�����āAtrue��߂�B
}
