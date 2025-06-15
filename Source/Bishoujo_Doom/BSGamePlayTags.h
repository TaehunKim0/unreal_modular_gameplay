#pragma once
#include "BSLogChannels.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "NativeGameplayTags.h"

namespace BSGamePlayTags
{
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	inline FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
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
