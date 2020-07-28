#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PostLoadListener.generated.h"

UINTERFACE(BlueprintType)
class UPostLoadListener : public UInterface
{
    GENERATED_UINTERFACE_BODY()
};

inline UPostLoadListener::UPostLoadListener(FObjectInitializer const& ObjectInitializer) { }

/**
 * This interface allows derived objects to run code once serialized data has been applied to it.
 * For example, the visibility of an actor is not saved by default. As an alternative, you could save a bool 'bInvisible' and reapply the visibility during the interface's PostLoad event.
 */
class IPostLoadListener
{
    GENERATED_IINTERFACE_BODY()
    
public:
    
    /**
     * Called after the UObject's `Serialize` method is called.
     *
     * Primarily used after spawning an actor from disk and loading its serialized data.
     * This method can allow an object to take action upon its new serialized data.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|Saving")
    void PostLoad();
};
