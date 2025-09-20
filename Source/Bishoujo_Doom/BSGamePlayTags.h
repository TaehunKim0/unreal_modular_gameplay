#pragma once
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "BSLogChannels.h"

namespace BSGamePlayTags
{
	BISHOUJO_DOOM_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_CharacterDefinitionLoaded);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_CharacterInitialized);
	BISHOUJO_DOOM_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

}
