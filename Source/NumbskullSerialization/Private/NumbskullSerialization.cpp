// Copyright 2019-2020 James Kelly, Michael Burdge

#include "NumbskullSerialization.h"

#define LOCTEXT_NAMESPACE "FNumbskullSerializationModule"

void FNumbskullSerializationModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FNumbskullSerializationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNumbskullSerializationModule, NumbskullSerialization)
