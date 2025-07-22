#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)	// 현재 모듈 내에서만 접근 가능하도록 제한함
class UItemInterface : public UInterface  // 리플렉션을 위한 인터페이스 클래스
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPARTAPROJECT_API IItemInterface
{
	GENERATED_BODY()


public:	// 자식들 무조건 구현 -> 순수가상함수들은 여기에
	UFUNCTION()	// 엔진의 리플렉션 시스템에 함수 등록 -> 엔진이 런타임 때 이 함수를 호출할 수 있도록
	virtual void OnItemOverlap(  // 아이템과 겹쳤을 때 호출되는 함수
		UPrimitiveComponent* OverlappedComp, // 오버랩이 발생한 자기 자신 = SphereComponent
		AActor* OtherActor,	// 겹친 다른 액터 = 플레이어 캐릭터
		UPrimitiveComponent* OtherComp, // 충돌 원인 컴포넌트 = 플레이어 캐릭터의 CapsuleComponent
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;	
	UFUNCTION()
	virtual void OnItemEndOverlap(  // 아이템과 겹침이 끝났을 때 호출되는 함수
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,	
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex) = 0;	
	virtual void ActivateItem(AActor* Activator) = 0;	// 아이템을 활성화할 때 호출되는 함수
	virtual FName GetItemType() const = 0;	// 아이템의 타입을 반환하는 함수 -> return만 하기 때문에 const
	// String 말고 빠른 FName을 사용
};
