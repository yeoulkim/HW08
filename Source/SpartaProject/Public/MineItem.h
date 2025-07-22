#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();	// 생성자

	USphereComponent* ExplosionCollision;	// 폭발 범위 콜리전 컴포넌트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;	// 파티클 시스템 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;	// 사운드 추가

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;	// 폭발 지연 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;	// 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;	// 폭발 피해

	bool  bHasExploded;
	
	FTimerHandle ExplosionTimerHandle;	// 폭발 타이머 핸들러 선언

	virtual void ActivateItem(AActor* Activator) override;

	void Explode();	// 폭발 함수
};
