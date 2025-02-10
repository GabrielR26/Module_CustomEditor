#pragma once

#include "CoreMinimal.h"
#include "CustomEditorSettings.generated.h"

#define SETTINGS_FILE_PATH FPaths::ProjectSavedDir() + "CustomEditor/Settings/Settings.json"

/** 
 * The settings of CustomEditor module
 */
UCLASS()
class CUSTOMEDITOR_API UCustomEditorSettings : public UClass
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggle, bool, _value);
	FOnToggle onToggle;

public:
	void InitSettings();

	FORCEINLINE FOnToggle& OnToggle() { return onToggle; }

#pragma region Combat
	UPROPERTY()
	bool toggle;

	bool GetToggle();

	void Toggle();

private:
	TSharedPtr<FJsonObject> ReadJson(FString JsonPath);
	void WriteJSon(FString JsonPath, TSharedPtr<FJsonObject> JsonObject);
#endif
};