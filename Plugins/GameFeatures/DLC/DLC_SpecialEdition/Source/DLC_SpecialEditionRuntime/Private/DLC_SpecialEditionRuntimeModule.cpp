// Copyright Epic Games, Inc. All Rights Reserved.

#include "DLC_SpecialEditionRuntimeModule.h"

#define LOCTEXT_NAMESPACE "FDLC_SpecialEditionRuntimeModule"

void FDLC_SpecialEditionRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory;
	// the exact timing is specified in the .uplugin file per-module
}

void FDLC_SpecialEditionRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLC_SpecialEditionRuntimeModule, DLC_SpecialEditionRuntime)
