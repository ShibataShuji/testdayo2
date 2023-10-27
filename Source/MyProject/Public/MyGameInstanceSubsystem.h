// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyGameInstanceSubsystem.generated.h"

class IWebSocket;

DECLARE_LOG_CATEGORY_EXTERN(LogMyGameInstance, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSockMsg, FString, msg);

/**
 * MyGameInstanceSubsystem
 */
UCLASS()
class UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;

	UPROPERTY(BlueprintAssignable)
	FSockMsg OnReceiveMessage;

private:
	TSharedPtr<IWebSocket> WebSocket = nullptr;
	FString message;

	void OnSocketConnected()const;
	void OnSocketConnectioinError(const FString& err)const;
	void OnSocketClosed(const int statusCode, const FString& reason, const bool wasClean)const;
	void OnSocketMessage(const FString& msg);
};