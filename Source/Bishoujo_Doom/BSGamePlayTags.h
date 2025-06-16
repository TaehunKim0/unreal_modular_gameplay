#pragma once
#include "BSLogChannels.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "NativeGameplayTags.h"

namespace BSGamePlayTags
{
	BISHOUJO_DOOM_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
}
