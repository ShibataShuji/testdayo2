// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

DEFINE_LOG_CATEGORY(LogMyGameInstance);

/**
 * @brief
 *  ��������
 * @param Collection
 */
void UMyGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection) {

	// Azure Web PubSub�̐ڑ������L�q
	const FString ServerUri = TEXT("wss://wstest01.webpubsub.azure.com/client/hubs/UE?access_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJ3c3M6Ly93c3Rlc3QwMS53ZWJwdWJzdWIuYXp1cmUuY29tL2NsaWVudC9odWJzL1VFIiwiaWF0IjoxNjk3Njc1OTc4LCJleHAiOjE2OTc3NjIzNzgsInJvbGUiOlsid2VicHVic3ViLnNlbmRUb0dyb3VwIiwid2VicHVic3ViLmpvaW5MZWF2ZUdyb3VwIl0sInN1YiI6IlVFIn0.P2kPyQ7p78pXmyjUyZoOFESlbaZrFXMxk-8B5pEdgiY");
	const FString ServerPrt = TEXT("json.webpubsub.azure.v1");

	// WebSocket����
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerUri, ServerPrt);

	// �C�x���g�n���h���[
	WebSocket->OnConnected().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketConnected);
	WebSocket->OnConnectionError().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketConnectioinError);
	WebSocket->OnClosed().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketClosed);
	WebSocket->OnMessage().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketMessage);

	// WebSocket�ڑ��{�O���[�v�ɏ���
	WebSocket->Connect();
	WebSocket->Send(TEXT("{\"type\":\"joinGroup\",\"group\":\"UE\"}"));
}

/**
 * @brief
 *  �I������
 */
void UMyGameInstanceSubsystem::Deinitialize() {
	WebSocket->Close();
	WebSocket = nullptr;
}

/**
 * @brief
 *  WebSocket�ɐڑ��������ɌĂяo����鏈��
 */
void UMyGameInstanceSubsystem::OnSocketConnected() const {
	UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Connected"));
}

/**
 * @brief
 *  WebSocket�̐ڑ��Ɏ��s�����Ƃ��ɌĂяo����鏈��
 * @param err
 */
void UMyGameInstanceSubsystem::OnSocketConnectioinError(const FString& err) const {
	UE_LOG(LogMyGameInstance, Warning, TEXT("WebSocket Error: %s"), *err);
}

/**
 * @brief
 *  WebSocket������ꂽ���ɌĂяo����鏈��
 * @param statusCode
 * @param reason
 * @param wasClean
 */
void UMyGameInstanceSubsystem::OnSocketClosed(const int statusCode, const FString& reason, const bool wasClean) const {
	UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Closed: status %d, reason %s"), statusCode, *reason);
}

/**
 * @brief
 *  WebSocket���烁�b�Z�[�W����M�������ɌĂяo����鏈��
 * @param msg
 */
void UMyGameInstanceSubsystem::OnSocketMessage(const FString& msg) {

	// ��M���b�Z�[�W���߂���
	message += msg;

	// JSON�ϊ�
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(*message);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	// JSON�ϊ����ł��āAdata�t�B�[���h�������Ă���ꍇ�́A��M����data�t�B�[���h��BP�ɓn��
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
		if (JsonObject->HasField("data")) {
			message = JsonObject->GetStringField("data");
			UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Message: %s"), *message);
			OnReceiveMessage.Broadcast(message);
		}
		message = "";
	}
}