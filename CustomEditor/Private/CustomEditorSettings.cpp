#include "CustomEditorSettings.h"
#if WITH_EDITORONLY_DATA
#include "Serialization/JsonSerializer.h"

#define TOGGLE_FIELD TEXT("Toggle")

void UCustomEditorSettings::InitSettings()
{
	// Create Json file if didn't exist
	if (!FPaths::FileExists(SETTINGS_FILE_PATH))
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetBoolField(TOGGLE_FIELD, false);
		toggle = false;
		WriteJSon(SETTINGS_FILE_PATH, JsonObject);
	}
	else // Get & update saved values
	{
		bool Result;
		TSharedPtr<FJsonObject> JsonObject = ReadJson(SETTINGS_FILE_PATH);
		bool Success = JsonObject->TryGetBoolField(TOGGLE_FIELD, Result);
		toggle = Success ? Result : false;
	}
}

bool UCustomEditorSettings::GetToggle()
{
	bool Result;
	TSharedPtr<FJsonObject> JsonObject = ReadJson(SETTINGS_FILE_PATH);
	const bool Success = JsonObject->TryGetBoolField(TOGGLE_FIELD, Result);
	if (Success)
		toggle = Result;
	return toggle;
}

void UCustomEditorSettings::Toggle()
{
	// Read value
	bool Result;
	TSharedPtr<FJsonObject> JsonObject = ReadJson(SETTINGS_FILE_PATH);
	const bool Success = JsonObject->TryGetBoolField(TOGGLE_FIELD, Result);
	if (Success)
	{
		toggle = !Result;
		onToggle.Broadcast(toggle);
		// Saved value
		JsonObject->SetBoolField(TOGGLE_FIELD, toggle);
		WriteJSon(SETTINGS_FILE_PATH, JsonObject);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Failed to get toggle field in json"));
}

TSharedPtr<FJsonObject> UCustomEditorSettings::ReadJson(FString JsonPath)
{
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *JsonPath);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonValue> JsonResult;

	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize Json string"));
		return nullptr;
	}

	return JsonObject;
}

void UCustomEditorSettings::WriteJSon(FString JsonPath, TSharedPtr<FJsonObject> JsonObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString, 0);

	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		FFileHelper::SaveStringToFile(JsonString, *JsonPath);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Failed to serialize Json string"));
}
#endif