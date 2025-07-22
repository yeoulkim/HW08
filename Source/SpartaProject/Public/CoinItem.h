#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ACoinItem : public ABaseItem

{
	GENERATED_BODY()


public:
	ACoinItem();	// 생성자

	virtual void ActivateItem(AActor* Activator) override;	// 아이템 활성화 함수 오버라이드

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;

};