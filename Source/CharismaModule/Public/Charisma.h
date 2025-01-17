#pragma once

#include "CharismaEvents.h"
#include "Client.h"
#include "CoreMinimal.h"
#include "Interfaces/IHttpResponse.h"
#include "Room.h"

#include "Charisma.generated.h"

enum ECharismaLogSeverity
{
	Info,
	Warning,
	Error
};

USTRUCT(BlueprintType)
struct FCharismaMessageHistoryResponse
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FCharismaMessage> Messages;
};

USTRUCT(BlueprintType)
struct FCharismaPlaythroughInfoResponse
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FCharismaEmotion> Emotions;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCharismaMemory> Memories;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTokenDelegate, FString, Token, int32, PlaythroughId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConversationDelegate, int32, ConversationId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageHistoryDelegate, const FCharismaMessageHistoryResponse&, MessageHistory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlaythroughInfoDelegate, const FCharismaPlaythroughInfoResponse&, PlaythroughInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectionDelegate, bool, IsConnected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTypingDelegate, bool, IsTyping);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageDelegate, const FCharismaMessageEvent&, MessageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FErrorDelegate, const FCharismaErrorEvent&, ErrorEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReadyDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CHARISMAMODULE_API UCharisma : public UObject
{
	GENERATED_BODY()

public:
	UCharisma();
	~UCharisma();

	// Static

	static const FString BaseURL;
	static const FString SocketURL;

	UFUNCTION(BlueprintCallable, Category = Charisma)
	static UCharisma* CreateCharismaObject(UObject* Owner);

	static void Log(const int32 Key, const FString& Message, const ECharismaLogSeverity Severity, const float Duration = 5.f);

	// Member

	UFUNCTION(BlueprintCallable, Category = Setup)
	void CreatePlaythroughToken(const int32 StoryId, const int32 StoryVersion, const FString& ApiKey) const;

	UFUNCTION(BlueprintCallable, Category = Setup)
	void CreateConversation(const FString& PlaythroughToken) const;

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void SetMemory(const FString& PlaythroughToken, const FString& RecallValue, const FString& SaveValue) const;

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void RestartFromEventId(const FString& PlaythroughToken, const int64 EventId) const;

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void GetMessageHistory(const FString& PlaythroughToken, const int32 ConversationId, const int64 MinEventId) const;

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void GetPlaythroughInfo(const FString& PlaythroughToken) const;

	UFUNCTION(BlueprintCallable, Category = Connection)
	void Connect(const FString& Token, const int32 PlaythroughId);

	UFUNCTION(BlueprintCallable, Category = Connection)
	void Disconnect();

	UFUNCTION(BlueprintCallable, Category = Play)
	void Action(const int32 ConversationId, const FString& ActionName) const;

	UFUNCTION(BlueprintCallable, Category = Play)
	void Start(const int32 ConversationId, const int32 SceneIndex, const int32 StartGraphId, const FString& StartGraphReferenceId,
		const bool UseSpeech = false);

	UFUNCTION(BlueprintCallable, Category = Play)
	void Reply(const int32 ConversationId, const FString& Message) const;

	UFUNCTION(BlueprintCallable, Category = Play)
	void Resume(const int32 ConversationId) const;

	UFUNCTION(BlueprintCallable, Category = Play)
	void Tap(const int32 ConversationId) const;

	UFUNCTION(BlueprintCallable, Category = Play)
	void ToggleSpeechOn();

	UFUNCTION(BlueprintCallable, Category = Play)
	void ToggleSpeechOff();

	// Properties

	UPROPERTY(BlueprintReadWrite)
	FString PlaythroughToken;

	// Events

	UPROPERTY(BlueprintAssignable, Category = Events)
	FTokenDelegate OnTokenCreated;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FConversationDelegate OnConversationCreated;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FMessageHistoryDelegate OnMessageHistory;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FPlaythroughInfoDelegate OnPlaythroughInfo;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FConnectionDelegate OnConnected;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FReadyDelegate OnReady;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FTypingDelegate OnTyping;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FMessageDelegate OnMessage;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FErrorDelegate OnError;

private:
	// Member

	void OnTokenRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	void OnConversationRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	void OnSetMemory(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	void OnRestartRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	void OnMessageHistoryComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	void OnPlaythroughInfoComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful) const;

	SpeechConfig GetSpeechConfig() const;

	// Properties

	TSharedPtr<Client> ClientInstance;

	TSharedPtr<Room<void>> RoomInstance;

	bool bUseSpeech = false;

	bool bIsPlaying = false;
};
