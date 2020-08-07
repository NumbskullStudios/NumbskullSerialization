// Copyright 2019-2020 James Kelly, Michael Burdge

#pragma once

#include "CoreMinimal.h"
#include "ObjectData.generated.h"

/**
 * Stores a TArray<uint8> captured (usually) from a UObject's Serialize method.
 */
USTRUCT(BlueprintType)
struct FObjectData
{
    GENERATED_BODY()
    
    /** Binary data to read back into an object or save to disk*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Numbskull")
	TArray<uint8> Data;
    
    friend FArchive& operator << (FArchive& Ar, FObjectData& Object)
    {
        Ar << Object.Data;
        return Ar;
    }
};
