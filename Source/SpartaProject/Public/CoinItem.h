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
	// 이동 상태를 설정하는 함수
	// 기본적으로 모든 코인에서 사용될 SetMove()
	virtual void SetMove(bool bMove);
	
	// 이동을 시작하는 함수
	virtual void StartMoving();
	// 이동을 멈추는 함수
	virtual void StopMoving();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;

	// 코인의 이동 상태를 추적하는 변수
	bool bIsMoving;  // true -> 이동 중, false -> 이동 멈춤
};