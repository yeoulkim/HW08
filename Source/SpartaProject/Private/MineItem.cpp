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
	// 원시 포인터로 참조되어 레벨 전환 시 가비지 컬렉팅이 이루어지지 않아 터진 것으로 예상.
	// 약한 참조로 변경하여 가비지 컬렉팅이 이루어지도록 수정
	// UParticleSystemComponent > Particle = nullptr
	// 언리얼 스마트 포인트
	// C++의 Weakptr인데, UObject 전용인 스마트 포인터.
	TWeakObjectPtr<UParticleSystemComponent> Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			true	// 자동 파괴되도록 변경해봄.... 될지 안될지는 테스트 해야함!!
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

	// 폭발 범위 안에 있는 액터들 전부 확인
	TArray<AActor*> OverlappingActors;
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

	if (Particle.Get())
	{
		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[Particle]()
			{
				//Pending kill
				if (IsValid(Particle.Get()))
				{
					Particle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}
