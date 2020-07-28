#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "Serializable.generated.h"

UINTERFACE(BlueprintType)
class USerializable : public UInterface
{
    GENERATED_UINTERFACE_BODY()
};

inline USerializable::USerializable(FObjectInitializer const& ObjectInitializer) { }

/**
 * This is the main interface an object should inherit from if it needs to save and load data.
 * However, objects using this interface should be 'static' objects - ie they're alive at the start of a level.
 * Dynamic objects could be loaded after OnLoad and therefore miss the trigger.
 * Use PostLoadListener in that case.
 *
 * OnSave -> The game is saving. The object should save itself, or any other object, to disk.
 * OnLoad -> The game is loading. The object should load its data from disk, or load other objects as needed.
 * OnSaved -> All objects are saved. A game manager might listen to this to unpause the menu.
 * OnLoaded -> All objects are loaded. Useful if you wanted to wait for the player to receive their equipment.
 */
class ISerializable
{
    GENERATED_IINTERFACE_BODY()
    
public:
    
    /**
     * Called when the game is saving to disk.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|Saving")
    void OnSave(const FString& GameName);
    
    /**
     * Called when the game is loading from disk.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|Saving")
    void OnLoad(const FString& GameName);
    
    /**
     * Called once all objects are saved.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|Saving")
    void OnSaved();
    
    /**
     * Called once all objects are loaded.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|Saving")
    void OnLoaded();
};
