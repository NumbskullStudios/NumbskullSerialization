// Copyright 2019-2020 James Kelly, Michael Burdge

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NewGameListener.generated.h"

UINTERFACE(BlueprintType)
class UNewGameListener : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

inline UNewGameListener::UNewGameListener(FObjectInitializer const& ObjectInitializer) { }

/**
 * This interface allows an object to carry out logic at each new game.
 * A new game being defined as the typical menu option 'New Game'.
 * Therefore, this interface won't be called every time a game is loaded.
 */
class INewGameListener
{
	GENERATED_IINTERFACE_BODY()

public:

	/**
	 * Called when the a new game is being created.
	 *
	 * Can be used to setup the game.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Numbskull|OnNewGame")
    void OnNewGame();
};
