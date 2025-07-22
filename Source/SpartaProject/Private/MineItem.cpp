#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpartaCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f; // 기본 폭발 지연 시간 설정
	ExplosionRadius = 300.0f; // 기본 폭발 범위 설정 
	ExplosionDamage = 30.0f; // 기본 폭발 피해 설정
	ItemType = "Mine"; // 아이템 타입 설정
    bHasExploded = false;// 오버랩 연속 안되게

	// 콜리전 컴포넌트 추가 (반경)
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);  // 300.0f 넣기
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 모든 동적 오버랩 허용
	ExplosionCollision->SetupAttachment(Scene);	// 루트 컴포넌트인 씬 컴포넌트에 부착

}

void AMineItem::ActivateItem(AActor* Activator)	// 활성화되고 5초 후에 폭발
{
    if (bHasExploded) return;

	Super::ActivateItem(Activator);
	// 게임 월드에는 각자의 타이머를 관리하는 타이머 매니저가 있음
	// 타이머 핸들러 : 각자의 타이머를 갖고 있음

	// 월드에서 타이머 매니저 부름
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,	// 타이머 달아줌
		this,  // 타이머가 끝나면 호출할 대상 : 이 객체에
		&AMineItem::Explode,  // 호출할 함수
		ExplosionDelay,  // 지연 시간
		false  // 반복 여부 (false면 한 번만 실행)
	);

    bHasExploded = true;
}

void AMineItem::Explode()
{
    // 이미 선언된 'Particle' 변수를 재사용
    UParticleSystemComponent* Particle = nullptr;  // 다시 초기화

    if (ExplosionParticle)
    {
        // 이미 선언된 변수 'Particle'을 재사용
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(), // GetActorLocation() 함수 호출
            GetActorRotation(), // GetActorRotation() 함수 호출
            false
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExplosionSound,
            GetActorLocation()
        );
    }

    TArray<AActor*> OverlappingActors;  // 범위 내 겹치는 액터 검색
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    // 범위 내 돌면서 태그 확인
    for (AActor* Actor : OverlappingActors)
    {
        if (Cast<ASpartaCharacter>(Actor))
        {
            UGameplayStatics::ApplyDamage(
                Actor,	// 데미지를 받을 액터
                ExplosionDamage,	// 데미지의 양
                nullptr,	// 데미지를 유발한 주체
                this,	// 데미지를 입힌 액터
                UDamageType::StaticClass()	// 데미지의 유형
            );
        }
    }

    DestroyItem();

    if (Particle)
    {
        FTimerHandle DestroyParticleTimerHandle;
        // 람다 함수 - 이름이 없다?
        // 직접 구현하긴 뭐하고 간단하게 함수같이 쓰고 싶을 때 람다 쓰기
        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleTimerHandle,
            [Particle]()
            {
                Particle->DestroyComponent();
            },
            2.0f,
            false
        );
    }
}
