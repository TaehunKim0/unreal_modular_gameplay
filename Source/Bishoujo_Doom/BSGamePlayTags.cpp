#include "BSGamePlayTags.h"

namespace BSGamePlayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_PlayerStateInitialized, "InitState.PlayerStateInitialized", "2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_InputComponentInitialized, "InitState.InputComponentInitialized", "3");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_ASCInitialized, "InitState.ASCInitialized", "4");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_CharacterDefinitionInitialized, "InitState.CharacterDefinitionInitialized", "5.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_CharacterComponentInitialized, "InitState.CharacterComponentInitialized", "6.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "7.");

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogBS, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

