// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Factories/Factory.h>
#include <UObject/Object.h>

#include "PixelStreamingStreamerVideoInputMediaCaptureFactory.generated.h"

UCLASS()
class UPixelStreamingStreamerVideoInputMediaCaptureFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	public:
		virtual FText GetDisplayName() const override;
		virtual uint32 GetMenuCategories() const override;
		virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
