// Copyright 2019-2020 James Kelly, Michael Burdge

#include "NumbskullSerializationBPLibrary.h"

// Interfaces
#include "PostLoadListener.h"

// Serialization Objects
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"

// UObject Serialization
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

// Compressed Serialization
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Misc/CompressionFlags.h"

// Unreal Types
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "Misc/FileHelper.h"

DEFINE_LOG_CATEGORY(Serializer);

UNumbskullSerializationBPLibrary::UNumbskullSerializationBPLibrary(const FObjectInitializer &ObjectInitializer)
: Super(ObjectInitializer)
{
}

//
// GLOBAL
//

bool UNumbskullSerializationBPLibrary::Serialize(TArray<uint8>& OutSerializedData, UObject* InObject)
{
    FMemoryWriter Writer(OutSerializedData, true);
    FObjectAndNameAsStringProxyArchive Archive(Writer, true);
    Writer.SetIsSaving(true);
    InObject->Serialize(Archive);
    return true;
}

bool UNumbskullSerializationBPLibrary::ApplySerialization(const TArray<uint8>& SerializedData, UObject* InObject)
{
    if (!InObject || InObject->IsPendingKill() || SerializedData.Num() <= 0)
    {
        UE_LOG(Serializer, Warning, TEXT("Couldn't apply serialized data on the object. Either it's null, being destroyed or there's no data"));
        return false;
    }
    
    FMemoryReader ActorReader(SerializedData, true);
    FObjectAndNameAsStringProxyArchive Archive(ActorReader, true);
    ActorReader.SetIsLoading(true);
    
    InObject->Serialize(Archive);
    
    if (InObject->GetClass()->ImplementsInterface(UPostLoadListener::StaticClass()))
    {
        IPostLoadListener::Execute_PostLoad(InObject);
    }
    
    return true;
}

bool UNumbskullSerializationBPLibrary::SaveBytesToDisk(const FString& InFileName, const TArray<uint8>& InBytes)
{
    if (InBytes.Num() == 0)
    {
        UE_LOG(Serializer, Warning, TEXT("No bytes to save to disk"));
        return false;
    }
    
    if (FFileHelper::SaveArrayToFile(InBytes, *InFileName))
    {
        UE_LOG(Serializer, Log, TEXT("Save Data To {%s} Successful"), *InFileName);
        return true;
    }
    return false;
}

bool UNumbskullSerializationBPLibrary::LoadBytesFromDisk(const FString& InFileName, TArray<uint8>& OutBytes)
{
    TArray<uint8> Bytes;
    
    if (!FFileHelper::LoadFileToArray(Bytes, *InFileName))
    {
        UE_LOG(Serializer, Error, TEXT("Load Failed. Couldn't read from file {%s}"), *InFileName);
        return false;
    }
    
    if (Bytes.Num() <= 0)
    {
        UE_LOG(Serializer, Log, TEXT("Load Data From {%s} Failed. No bytes found"), *InFileName);
        return false;
    }
    
    UE_LOG(Serializer, Log, TEXT("Load Data From {%s} Successful"), *InFileName);
    
    OutBytes = Bytes;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::SaveArchiveToDisk(const FString& InFileName, const FBufferArchive& InArchive)
{
    return SaveBytesToDisk(InFileName, InArchive);
}

bool UNumbskullSerializationBPLibrary::SaveArchiveToDiskCompressed(const FString& InFileName, FBufferArchive InArchive)
{
    TArray<uint8> CompressedData;
    FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, NAME_Zlib);
    
    Compressor << InArchive;
    
    Compressor.Flush();
    
    return SaveBytesToDisk(InFileName, CompressedData);
}

//
// ACTOR PROXIES
//

bool UNumbskullSerializationBPLibrary::SaveActor(AActor* InActorToSave, FActorProxy& OutActorProxy)
{
    if (ensure(InActorToSave))
    {
        // Pawns forget or override new controllers upon level loads
        // To avoid this, unpossess before saving
        APawn* Pawn = Cast<APawn>(InActorToSave);
        AController* Controller = Pawn ? Pawn->Controller : nullptr;
        
        if (Pawn && Controller)
        {
            UE_LOG(Serializer, Log, TEXT("Unpossessing pawn before saving"));
            Pawn->Controller->UnPossess();
        }
        
        FActorProxy ActorProxy;
        ActorProxy.ActorName = InActorToSave->GetFName();
        ActorProxy.ActorClass = InActorToSave->GetClass()->GetPathName();
        ActorProxy.ActorTransform = InActorToSave->GetTransform();
        
        Serialize(ActorProxy.ActorData, InActorToSave);
        
        OutActorProxy = ActorProxy;
        
        // If we are only saving the level, we need to repossess the pawns
        if (Pawn && Controller)
        {
            UE_LOG(Serializer, Log, TEXT("Saved and repossessing pawn"));
            Controller->Possess(Pawn);
        }
        
        check(!ActorProxy.ActorClass.IsEmpty());
        
        return true;
    }
    UE_LOG(Serializer, Warning, TEXT("Actor was null and couldn't save"));
    return false;
}

bool UNumbskullSerializationBPLibrary::LoadActor(const UObject* WorldContextObject, const FActorProxy& InActorProxy, AActor*& OutLoadedActor)
{
    check(WorldContextObject);
    
    UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    
    check(World);
    check(!InActorProxy.ActorClass.IsEmpty());
    check(InActorProxy.ActorData.Num() > 0);
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = InActorProxy.ActorName;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.OverrideLevel = World->PersistentLevel;
    SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
    UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *InActorProxy.ActorClass);
    
    if (!SpawnClass)
    {
        SpawnClass = StaticLoadClass(AActor::StaticClass(), nullptr, *InActorProxy.ActorClass);
    }
    
    if (SpawnClass)
    {
        AActor* SpawnedActor = World->SpawnActor(SpawnClass, &InActorProxy.ActorTransform, SpawnParams);
        
        if (!SpawnedActor)
        {
            UE_LOG(Serializer, Error, TEXT("Spawn actor failed"));
            return false;
        }
        
        ApplySerialization(InActorProxy.ActorData, SpawnedActor);
        
        OutLoadedActor = SpawnedActor;
        return true;
    }
    UE_LOG(Serializer, Warning, TEXT("Couldn't spawn actor because the class couldn't be found"));
    return false;
}

bool UNumbskullSerializationBPLibrary::SaveActorProxyToDisk(const FString& InFileName, FActorProxy InActorProxy)
{
    FBufferArchive BinaryData;
    BinaryData << InActorProxy;
    
    return SaveArchiveToDisk(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::SaveActorProxyToDiskCompressed(const FString& InFileName, FActorProxy InActorProxy)
{
    FBufferArchive BinaryData;
    BinaryData << InActorProxy;
    
    return SaveArchiveToDiskCompressed(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::LoadActorProxyFromDisk(const FString& InFileName, FActorProxy& OutActorProxy)
{
    TArray<uint8> Bytes;
    
    if (!LoadBytesFromDisk(InFileName, Bytes))
    {
        return false;
    }
    
    FMemoryReader FromBinary = FMemoryReader(Bytes, true);
    FromBinary.Seek(0);
    
    FActorProxy ActorProxy;
    FromBinary << ActorProxy;
    
    OutActorProxy = ActorProxy;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::LoadActorProxyFromDiskCompressed(const FString& InFileName, FActorProxy& OutActorProxy)
{    
    TArray<uint8> Bytes;
    
    LoadBytesFromDisk(InFileName, Bytes);
    
    FArchiveLoadCompressedProxy Decompressor =
    FArchiveLoadCompressedProxy(Bytes, NAME_Zlib);
    
    if(Decompressor.GetError())
    {
        UE_LOG(Serializer, Error, TEXT("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed"));
        return false;
    }
    
    FBufferArchive DecompressedBinaryArray;
    Decompressor << DecompressedBinaryArray;
    
    FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true);
    FromBinary.Seek(0);
    
    FActorProxy ActorProxy;
    FromBinary << ActorProxy;
    
    OutActorProxy = ActorProxy;
    
    return true;
}

//
// UOBJECTS
//

bool UNumbskullSerializationBPLibrary::SaveObject(UObject* InObject, FObjectData& OutObjectData)
{
    if (InObject == nullptr)
    {
        UE_LOG(Serializer, Warning, TEXT("Object is null. Can't save"));
        return false;
    }
    
    FObjectData ObjectData;
    
    Serialize(ObjectData.Data, InObject);
    
    OutObjectData = ObjectData;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::LoadObject(UObject* InObject, FObjectData InObjectData)
{
    return ApplySerialization(InObjectData.Data, InObject);
}

bool UNumbskullSerializationBPLibrary::SaveObjects(const TArray<UObject*>& InObjects, FObjectData& OutObjectData)
{
    if (InObjects.Num() == 0)
    {
        UE_LOG(Serializer, Error, TEXT("No objects to save"));
        return false;
    }
    
    FObjectData ObjectData;
    
    // We can't use the serialize method as it'd override the bytes, rather than adding to it
    FMemoryWriter Writer(ObjectData.Data, true);
    FObjectAndNameAsStringProxyArchive Archive(Writer, true);
    Archive.SetIsSaving(true);
    
    for (UObject* Object : InObjects)
    {
        if (Object)
        {
            Object->Serialize(Archive);
        }
    }
    
    OutObjectData = ObjectData;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::LoadObjects(const TArray<UObject*>& InObjects, FObjectData InObjectData)
{
    if (InObjects.Num() == 0)
    {
        UE_LOG(Serializer, Error, TEXT("No objects to load"));
        return false;
    }
    
    if (InObjectData.Data.Num() == 0)
    {
        UE_LOG(Serializer, Error, TEXT("Binary array is empty. Can't load objects"));
        return false;
    }
    
    FMemoryReader ActorReader (InObjectData.Data, true);
    FObjectAndNameAsStringProxyArchive Archive(ActorReader, true);
    ActorReader.SetIsLoading(true);
    
    for (UObject* Object : InObjects)
    {
        if (Object)
        {
            Object->Serialize(Archive);
            
            if (Object->GetClass()->ImplementsInterface(UPostLoadListener::StaticClass()))
            {
                IPostLoadListener::Execute_PostLoad(Object);
            }
        }
    }
    
    return true;
}

bool UNumbskullSerializationBPLibrary::SaveObjectDataToDisk(const FString& InFileName, FObjectData InObjectData)
{
    FBufferArchive BinaryData;
    BinaryData << InObjectData;
    
    return SaveArchiveToDisk(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::LoadObjectDataFromDisk(const FString& InFileName, FObjectData& OutObjectData)
{
    TArray<uint8> Bytes;
    
    if (!LoadBytesFromDisk(InFileName, Bytes))
    {
        return false;
    }
    
    FMemoryReader FromBinary = FMemoryReader(Bytes, true);
    FromBinary.Seek(0);
    
    FObjectData ObjectData;
    FromBinary << ObjectData;
    
    OutObjectData = ObjectData;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::SaveObjectDataToDiskCompressed(const FString& InFileName, FObjectData InObjectData)
{
    FBufferArchive BinaryData;
    BinaryData << InObjectData;
    
    return SaveArchiveToDiskCompressed(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::LoadObjectDataFromDiskCompressed(const FString& InFileName, FObjectData& OutObjectData)
{
    TArray<uint8> Bytes;
    
    LoadBytesFromDisk(InFileName, Bytes);
    
    FArchiveLoadCompressedProxy Decompressor =
    FArchiveLoadCompressedProxy(Bytes, NAME_Zlib);
    
    if(Decompressor.GetError())
    {
        UE_LOG(Serializer, Error, TEXT("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed"));
        return false;
    }
    
    FBufferArchive DecompressedBinaryArray;
    Decompressor << DecompressedBinaryArray;
    
    FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true);
    FromBinary.Seek(0);
    
    FObjectData ObjectData;
    FromBinary << ObjectData;
    
    OutObjectData = ObjectData;
    
    return true;
}

//
// ACTORS
//

bool UNumbskullSerializationBPLibrary::SaveActorData(AActor* InActorToSave, FActorData& OutActorData)
{
    FActorData ActorData;
    
    Serialize(ActorData.Data, InActorToSave);
    ActorData.Transform = InActorToSave->GetTransform();
    
    OutActorData = ActorData;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::LoadActorData(AActor* InActorToLoad, const FActorData& InActorData)
{
    if (ApplySerialization(InActorData.Data, InActorToLoad))
    {
        InActorToLoad->SetActorTransform(InActorData.Transform);
        return true;
    }
    return false;
}

bool UNumbskullSerializationBPLibrary::SaveActorDataToDisk(const FString& InFileName, FActorData InActorData)
{
    FBufferArchive BinaryData;
    BinaryData << InActorData;
    
    return SaveArchiveToDisk(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::LoadActorDataFromDisk(const FString& InFileName, FActorData& OutActorData)
{
    TArray<uint8> Bytes;
    
    if (!LoadBytesFromDisk(InFileName, Bytes))
    {
        return false;
    }
    
    FMemoryReader FromBinary = FMemoryReader(Bytes, true);
    FromBinary.Seek(0);
    
    FActorData ActorData;
    FromBinary << ActorData;
        
    OutActorData = ActorData;
    
    return true;
}

bool UNumbskullSerializationBPLibrary::SaveActorDataToDiskCompressed(const FString& InFileName, FActorData InActorData)
{
    FBufferArchive BinaryData;
    BinaryData << InActorData;
    
    return SaveArchiveToDiskCompressed(InFileName, BinaryData);
}

bool UNumbskullSerializationBPLibrary::LoadActorDataFromDiskCompressed(const FString& InFileName, FActorData& OutActorData)
{
    TArray<uint8> Bytes;
    
    LoadBytesFromDisk(InFileName, Bytes);
    
    FArchiveLoadCompressedProxy Decompressor =
    FArchiveLoadCompressedProxy(Bytes, NAME_Zlib);
    
    if(Decompressor.GetError())
    {
        UE_LOG(Serializer, Error, TEXT("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed"));
        return false;
    }
    
    FBufferArchive DecompressedBinaryArray;
    Decompressor << DecompressedBinaryArray;
    
    FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true);
    FromBinary.Seek(0);
    
    FActorData ActorData;
    FromBinary << ActorData;
    
    OutActorData = ActorData;
    
    return true;
}
