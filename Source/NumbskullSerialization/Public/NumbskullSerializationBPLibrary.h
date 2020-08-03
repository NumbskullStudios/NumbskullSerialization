// Copyright 2019-2020 James Kelly, Michael Burdge

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

// Storage Types
#include "ActorProxy.h"
#include "ObjectData.h"
#include "ActorData.h"

#include "NumbskullSerializationBPLibrary.generated.h"

class FBufferArchive;
class FMemoryReader;

DECLARE_LOG_CATEGORY_EXTERN(Serializer, Log, All);

/**
 * Contains all methods for serializing data and saving and loading from disk.
 */
UCLASS()
class UNumbskullSerializationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
    
public:
    
    //
    // GLOBAL
    //
    
    /**
     * Serializes a UObject entirely and returns the serialized data.
     *
     * @param InObject Object to serialize.
     * @param OutSerializedData Serialized data if serialization was successful.
     *
     * @return True if successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving")
    static bool Serialize(TArray<uint8>& OutSerializedData, UObject* InObject);
    
    /**
     * Applies serialized data to an object.
     *
     * @param InActorProxy ActorProxy that provides the serialized data.
     * @param InActor Actor to apply the serialized data to.
     *
     * @return True if the serialized data was applied successfully, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving")
    static bool ApplySerialization(const TArray<uint8>& SerializedData, UObject* InObject);
    
    /**
     * Saves an array of bytes to a file.
     *
     * @param InFileName Full file name and path to save to.
     * @param InBytes Bytes to save to file.
     *
     * @return True if save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving")
    static bool SaveBytesToDisk(const FString& InFileName, const TArray<uint8>& InBytes);
    
    /**
     * Loads a file into an array of bytes.
     *
     * @param InFileName Full file name and path to load.
     * @param OutBytes The bytes loaded.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving")
    static bool LoadBytesFromDisk(const FString& InFileName, TArray<uint8>& OutBytes);
    
    /**
     * Saves an archive to disk.
     *
     * @param InFileName Full file name and path to save to.
     * @param InArchive Archive to save to file.
     *
     * @return True if save was successful, false if otherwise
     */
    static bool SaveArchiveToDisk(const FString& InFileName, const FBufferArchive& InArchive);
    
    /**
     * Compresses the archive before saving to disk.
     *
     * @param InFileName Full file name and path to save to.
     * @param InArchive Archive to save to file.
     *
     * @return True if save was successful, false if otherwise
     */
    static bool SaveArchiveToDiskCompressed(const FString& InFileName, FBufferArchive InArchive);
    
public:
    
    //
    // ACTOR PROXIES
    //
    
    /**
     * Completey saves an actor ready for saving to disk.
     *
     * The resulting object can be fully restored once loaded.
     *
     * @param InActorToSave The actor that will be saved.
     * @param OutActorProxy The resulting object that can be used to restore the actor.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|Actors")
    static bool SaveActor(AActor* InActorToSave, FActorProxy& OutActorProxy);
    
    /**
     * Spawns and loads an actor from an actor proxy.
     *
     * Uses the calling object to determine the world, rather than @see LoadActor, which explicity takes a UWorld parameter.
     *
     * @param WorldContextObject Current world context
     * @param InActorProxy Actor proxy to load.
     * @param OutLoadedActor Spawned and loaded actor.
     *
     * @return True if load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|Actors", meta=(WorldContext = "WorldContextObject"))
    static bool LoadActor(const UObject* WorldContextObject, const FActorProxy& InActorProxy, AActor*& OutLoadedActor);
    
    /**
     * Takes an actor proxy and saves it to disk.
     *
     * @param InFileName Full file name and path to save to.
     * @param InActorProxy The actor proxy to save to disk.
     *
     * @return True if the save to disk was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorProxy")
    static bool SaveActorProxyToDisk(const FString& InFileName, FActorProxy InActorProxy);
    
    /**
     * Loads an actor proxy from disk.
     *
     * @param InFileName Full file name and path to load from.
     * @param OutActorProxy The actor proxy loaded.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorProxy")
    static bool LoadActorProxyFromDisk(const FString& InFileName, FActorProxy& OutActorProxy);
    
    /**
     * Takes an actor proxy and saves it to disk compressed.
     *
     * @param InFileName Full file name and path to save to.
     * @param InActorProxy The actor proxy to save to disk.
     *
     * @return True if the save to disk was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorProxy|Compressed")
    static bool SaveActorProxyToDiskCompressed(const FString& InFileName, FActorProxy InActorProxy);
    
    /**
     * Loads a compressed actor proxy from disk.
     *
     * @param InFileName Full file name and path to load from.
     * @param OutActorProxy The actor proxy loaded.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorProxy|Compressed")
    static bool LoadActorProxyFromDiskCompressed(const FString& InFileName, FActorProxy& OutActorProxy);
    
public:
    
    //
    // UOBJECTS
    //
    
    /**
     * Saves an object's data into a new FObjectData object.
     *
     * MUST USE the corresponding LoadObject function when loading.
     *
     * @param InObject Object to save.
     * @param OutObjectData The created object containing binary data of the serialized object.
     *
     * @return True if successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool SaveObject(UObject* InObject, FObjectData& OutObjectData);
    
    /**
     * Loads an object's data from the FObjectData object.
     *
     * @param InObject Object to load.
     * @param InObjectData Object to load from.
     *
     * @return True if successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool LoadObject(UObject* InObject, FObjectData InObjectData);
    
    /**
     * Saves an array of objects into an FObjectData object.
     *
     * MUST USE LoadObjects to get loading in the correct order.
     *
     * @param InObjects Array of objects to serialize.
     * @param InObjectData The resultant object with binrary data.
     *
     * @return True if successful, false if otherwise
     * @see LoadObjects
     * @seealso SaveObject, LoadObject
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool SaveObjects(const TArray<UObject*>& InObjects, FObjectData& OutObjectData);
    
    /**
     * Loads an array of objects from an FObjectData object.
     *
     * @param InObjects Array of objects to load.
     * @param InObjectData The object with binary data to load from.
     *
     * @return True if successful, false if otherwise
     * @see SaveObjects
     * @seealso SaveObject, LoadObject
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool LoadObjects(const TArray<UObject*>& InObjects, FObjectData InObjectData);
    
    /**
     * Save an object data struct to a file on disk.
     *
     * @param InFileName Full file path to save to.
     * @param InObjectData Struct to save to file.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool SaveObjectDataToDisk(const FString& InFileName, FObjectData InObjectData);
    
    /**
     * Load an object data struct from a file on disk.
     *
     * @param InFileName Full file path to load.
     * @param OutObjectData Loaded object data struct.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData")
    static bool LoadObjectDataFromDisk(const FString& InFileName, FObjectData& OutObjectData);
    
    /**
     * Save an object data struct to a file on disk compressed.
     *
     * @param InFileName Full file path to save to.
     * @param InObjectData Struct to save to file.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData|Compressed")
    static bool SaveObjectDataToDiskCompressed(const FString& InFileName, FObjectData InObjectData);
    
    /**
     * Load a compressed object data struct from a file on disk.
     *
     * @param InFileName Full file path to load.
     * @param OutObjectData Loaded object data struct.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ObjectData|Compressed")
    static bool LoadObjectDataFromDiskCompressed(const FString& InFileName, FObjectData& OutObjectData);
    
public:
    
    //
    // ACTORS
    //
    
    /**
     * Saves and stores an actor's data in an FActorData struct.
     *
     * Similar to @see SaveActor but doesn't save its class.
     *
     * @param InActorToSave Actor to serialize.
     * @param OutActorData Resulting FActorData from serializing the actor.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool SaveActorData(AActor* InActorToSave, FActorData& OutActorData);
    
    /**
     * Loads an actor's data from an FActorData struct.
     *
     * Similar to @see LoadActor but doesn't spawn the actor; it only applies the data.
     *
     * @param InActorToLoad Actor to serialize.
     * @param ActorData FAtorData to read serialized data from.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool LoadActorData(AActor* InActorToLoad, const FActorData& InActorData);
    
    /**
     * Save an actor data struct to a file on disk.
     *
     * @param InFileName Full file path to save to.
     * @param InActorData Struct to save to file.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool SaveActorDataToDisk(const FString& InFileName, FActorData InActorData);
    
    /**
     * Load an actor data struct from a file on disk.
     *
     * @param InFileName Full file path to load.
     * @param OutActorData Loaded actor data struct.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool LoadActorDataFromDisk(const FString& InFileName, FActorData& OutActorData);
    
    /**
     * Save an actor data struct to a file on disk.
     *
     * @param InFileName Full file path to save to.
     * @param InActorData Struct to save to file.
     *
     * @return True if the save was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool SaveActorDataToDiskCompressed(const FString& InFileName, FActorData InActorData);
    
    /**
     * Load an actor data struct from a file on disk.
     *
     * @param InFileName Full file path to load.
     * @param OutActorData Loaded actor data struct.
     *
     * @return True if the load was successful, false if otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "Numbskull|Saving|ActorData")
    static bool LoadActorDataFromDiskCompressed(const FString& InFileName, FActorData& OutActorData);
};
