# Numbskull Serialization
## A simple and free serialization plugin

This plugin is currently used in the unreleased game _Beyond Binary_ by _Numbskull Studios_. The plugin was created because _Unreal_'s _SaveGame_ system didn't meet the requirments for _Beyond Binary_.

The majority of code sits inside the _NumbskullSerializationBPLibrary_ file with simple methods like _SaveActor_ and _SaveActorProxyToDisk_.

**Warning:** This plugin does not try to solve file/path/game management. Saving and loading methods expect full file paths so the user must properly supply these.

In _Beyond Binary_, the game instance collects all objects with the `Serializable` interface and passes the game name ("Game 1", for example).

The interface objects then use methods in a `GamePaths` object to convert game names into paths ("/path/to/project/folder/Saved/Games/Game 1/").

![Features](Documentation/AllFeatures.png)

## Brief Overview

![Brief Overview](Documentation/SimpleSaving.png)

#### Bool Returns

Each method in the library returns a bool to indicate success or failure. More logging will be added to help with debugging.

#### Storage Types

The library includes three main object types for storing data from an object or actor:

- **Actor Data** (Holds serialized data and the actor's transform)
- **Actor Proxy** (Holds serialized data as well as class, name and transform)
- **Object Data** (Holds only serialized data)

Methods like `SaveActor`, `SaveActorData` and `SaveObjectData` will return these storage types.

These storage types can be easily saved with `SaveActorProxyToDisk`, `SaveActorDataToDisk` and `SaveObjectDataToDisk`.

## How To Use (Object Data)

- Include `NumbskullSerializationBPLibrary.h`
- Add the `ISerializable` interface to your class
- Declare in the header

```
virtual void OnSave_Implementation(const FString& GameName) override;
virtual void OnLoad_Implementation(const FString& GameName) override;
```
- Save `this` object with

```
FObjectData ObjectData;
UNumbskullSerializationBPLibrary::SaveObject(this, ObjectData);
UNumbskullSerializationBPLibrary::SaveObjectDataToDisk(GameName + TEXT("MyFile.dat"), ObjectData);
```

- Load `this` object with

```
FObjectData ObjectData;
UNumbskullSerializationBPLibrary::LoadObjectDataFromDisk(GameName + TEXT("MyFile.dat"), ObjectData);
UNumbskullSerializationBPLibrary::LoadObject(this, ObjectData);
```

## How To Use (Actor Data)

- Include `NumbskullSerializationBPLibrary.h`
- Add the `ISerializable` interface to your class
- Declare in the header

```
virtual void OnSave_Implementation(const FString& GameName) override;
virtual void OnLoad_Implementation(const FString& GameName) override;
```
- Save `this` actor with

```
FObjectData ObjectData;
UNumbskullSerializationBPLibrary::SaveActorData(this, ObjectData);
UNumbskullSerializationBPLibrary::SaveActorDataToDisk(GameName + TEXT("MyFile.dat"), ObjectData);
```

- Load `this` actor with

```
FObjectData ObjectData;
UNumbskullSerializationBPLibrary::LoadActorDataFromDisk(GameName + TEXT("MyFile.dat"), ObjectData);
UNumbskullSerializationBPLibrary::LoadActorData(this, ObjectData);
```

## How To Use (Actor Proxy)

`ActorProxies`s save the entirety of an actor so it can be respawned during loading.
The intended use is for dynamically spawned enemies or equipment on the player like a sword.

It's best practice for an object like the `GameMode` to save and load these objects as, while a dynamically created object can save itself, they can't load themselves as they won't exist on new level loads.

- Include `NumbskullSerializationBPLibrary.h`
- Add the `ISerializable` interface to your class
- Declare in the header

```
virtual void OnSave_Implementation(const FString& GameName) override;
virtual void OnLoad_Implementation(const FString& GameName) override;
```
- Save a dynamically created actor with

```
AActor* ActorToSave;

FActorProxy ActorProxy;
UNumbskullSerializationBPLibrary::SaveActor(ActorToSave, ActorProxy);
UNumbskullSerializationBPLibrary::SaveActorProxyToDisk(GameName + TEXT("Actor.dat"), ActorProxy);
```

- Load the dynamically created actor

```
AActor* ActorToLoad = nullptr;

FActorProxy ActorProxy;
UNumbskullSerializationBPLibrary::LoadActorDataFromDisk(GameName + TEXT("Actor.dat"), ActorProxy);
UNumbskullSerializationBPLibrary::LoadActor(GetWorld(), ActorProxy, ActorToLoad);

ActorToLoad->AnyMethodAsTheActorIsLoaded();
```

## Full Saving and Loading Example

**MyActor.h**

```
#include "NumbskullSerializationBPLibrary.h"

class AMyActor : AActor, ISerializable
{
	virtual void OnSave_Implementation(const FString& GameName) override;
	virtual void OnLoad_Implementation(const FString& GameName) override;
};
```

**MyActor.cpp**

```
#include "MyActor.h"

void AMyActor::OnSave_Implementation(const FString& GameName)
{
    FString FileName = GameName + TEXT("MyActor.dat");
    
    FActorData ActorData;
    
    UNumbskullSerializationBPLibrary::SaveActorData(this, ActorData);
    UNumbskullSerializationBPLibrary::SaveObjectDataToDiskCompressed(FileName, ActorData);
}

void AMyActor::OnLoad_Implementation(const FString& GameName)
{
    FString FileName = GameName + TEXT("MyActor.dat");
    
    FActorData ActorData;

    UNumbskullSerializationBPLibrary::LoadObjectDataFromDiskCompressed(FileName, ActorData);
    UNumbskullSerializationBPLibrary::LoadObject(this, ActorData);
}
```

## On Load Interface

The library also includes a `PostLoadListener`. This interface allows an object to take action after its `Serialize` method is called. Intended for when you want to make sure all data has been loaded before proceeding.

In _Beyond Binary_, the method is used to wait until a robot's appearance has been loaded. Once we have the specific limbs, we can update the visuals. Doing this in `BeginPlay` would result in a "prefab" looking robot.

## New Game Interface

Finally, the library includes a `NewGameListener`. The interface allows objects to react to a new game. An example could be giving the player default equipment.
