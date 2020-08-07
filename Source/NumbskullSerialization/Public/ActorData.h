// Copyright 2019-2020 James Kelly, Michael Burdge

#pragma once

#include "CoreMinimal.h"
#include "ActorData.generated.h"

/**
 * Simple Blueprint usable object that stores an actor's serialized data as well as its transform.
 * Use ObjectData if you don't need to store the transform.
 */
USTRUCT(BlueprintType)
struct FActorData
{
    GENERATED_BODY()

    /** Binary data to read back into an object or save to disk*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Numbskull")
    TArray<uint8> Data;
    
    /** Transform of the actor*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Numbskull")
    FTransform Transform;
    
    friend FArchive& operator << (FArchive& Ar, FActorData& Object)
    {
        Ar << Object.Data;
        Ar << Object.Transform;
        return Ar;
    }
};
