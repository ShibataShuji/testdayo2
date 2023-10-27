// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

DEFINE_LOG_CATEGORY(LogMyGameInstance);

/**
 * @brief
 *  初期処理
 * @param Collection
 */
void UMyGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection) {

	// Azure Web PubSubの接続情報を記述
	const FString ServerUri = TEXT("wss://wstest01.webpubsub.azure.com/client/hubs/UE?access_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOiJ3c3M6Ly93c3Rlc3QwMS53ZWJwdWJzdWIuYXp1cmUuY29tL2NsaWVudC9odWJzL1VFIiwiaWF0IjoxNjk3Njc1OTc4LCJleHAiOjE2OTc3NjIzNzgsInJvbGUiOlsid2VicHVic3ViLnNlbmRUb0dyb3VwIiwid2VicHVic3ViLmpvaW5MZWF2ZUdyb3VwIl0sInN1YiI6IlVFIn0.P2kPyQ7p78pXmyjUyZoOFESlbaZrFXMxk-8B5pEdgiY");
	const FString ServerPrt = TEXT("json.webpubsub.azure.v1");

	// WebSocket生成
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerUri, ServerPrt);

	// イベントハンドラー
	WebSocket->OnConnected().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketConnected);
	WebSocket->OnConnectionError().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketConnectioinError);
	WebSocket->OnClosed().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketClosed);
	WebSocket->OnMessage().AddUObject(this, &UMyGameInstanceSubsystem::OnSocketMessage);

	// WebSocket接続＋グループに所属
	WebSocket->Connect();
	WebSocket->Send(TEXT("{\"type\":\"joinGroup\",\"group\":\"UE\"}"));
}

/**
 * @brief
 *  終了処理
 */
void UMyGameInstanceSubsystem::Deinitialize() {
	WebSocket->Close();
	WebSocket = nullptr;
}

/**
 * @brief
 *  WebSocketに接続した時に呼び出される処理
 */
void UMyGameInstanceSubsystem::OnSocketConnected() const {
	UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Connected"));
}

/**
 * @brief
 *  WebSocketの接続に失敗したときに呼び出される処理
 * @param err
 */
void UMyGameInstanceSubsystem::OnSocketConnectioinError(const FString& err) const {
	UE_LOG(LogMyGameInstance, Warning, TEXT("WebSocket Error: %s"), *err);
}

/**
 * @brief
 *  WebSocketが閉じられた時に呼び出される処理
 * @param statusCode
 * @param reason
 * @param wasClean
 */
void UMyGameInstanceSubsystem::OnSocketClosed(const int statusCode, const FString& reason, const bool wasClean) const {
	UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Closed: status %d, reason %s"), statusCode, *reason);
}

/**
 * @brief
 *  WebSocketからメッセージを受信した時に呼び出される処理
 * @param msg
 */
void UMyGameInstanceSubsystem::OnSocketMessage(const FString& msg) {

	// 受信メッセージ溜めこみ
	message += msg;

	// JSON変換
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(*message);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	// JSON変換ができて、dataフィールドを持っている場合は、受信したdataフィールドをBPに渡す
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
		if (JsonObject->HasField("data")) {
			message = JsonObject->GetStringField("data");
			UE_LOG(LogMyGameInstance, Log, TEXT("WebSocket Message: %s"), *message);
			OnReceiveMessage.Broadcast(message);
		}
		message = "";
	}
}