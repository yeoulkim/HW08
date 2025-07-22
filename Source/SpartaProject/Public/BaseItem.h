#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"  // 아이템 인터페이스를 포함시켜야 함
#include "BaseItem.generated.h"
 
class USphereComponent;  // 스피어 컴포넌트 미리 선언 필요
UCLASS()
class SPARTAPROJECT_API ABaseItem : public AActor, public IItemInterface  // IItemInterface를 상속받아야 함
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item") // 리플렉션으로 에디터에 노출
	FName ItemType;  // 아이템의 타입을 저장하는 변수

	// 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component") // 객체 자체 변경 x, 내부 속성은 에디터에서 조정 가능
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* PickupParticle;	// 파티클 시스템 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* PickupSound;	// 사운드 추가


	// 인터페이스에 있는 순수가상함수들을 구현해야 함
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,	
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;

	virtual void DestroyItem();

};
