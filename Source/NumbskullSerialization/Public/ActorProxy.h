// Copyright 2019-2020 James Kelly, Michael Burdge

#pragma once

#include "CoreMinimal.h"
#include "ActorProxy.generated.h"

/**
 * Serializable representation of an actor that can be saved and loaded from disk.
 *
 * Stores information about the actor's class so the entire actor can be fully loaded from disk.
 */
USTRUCT(BlueprintType)
struct FActorProxy
{
	GENERATED_BODY()

    /** Class of the actor. This gives us the 'chassis' to add serialized data to*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Numbskull)
    FString ActorClass;

    /** Name of the actor. Less important but makes a newly loaded actor seem more like its former self*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Numbskull)
    FName ActorName;

    /** Transform of the actor. Gives us the position, rotation and scale information*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Numbskull)
    FTransform ActorTransform;

    /** Serialized data. This gives us the details of the actor like their health, equipment etc.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Numbskull)
    TArray<uint8> ActorData;
    
    friend FArchive& operator<<(FArchive& Ar, FActorProxy& ActorProxy)
    {
        Ar << ActorProxy.ActorClass;
        Ar << ActorProxy.ActorName;
        Ar << ActorProxy.ActorTransform;
        Ar << ActorProxy.ActorData;
        return Ar;
    }
};
