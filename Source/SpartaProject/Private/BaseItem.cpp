#include "BaseItem.h"
#include "Components/SphereComponent.h"  // 스피어 컴포넌트 헤더 포함
#include "Kismet/GameplayStatics.h"	// 파티클, 사운드
#include "Particles/ParticleSystemComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));	// 객체 생성
	SetRootComponent(Scene);	// 항상 Scene을 루트 컴포넌트로 설정

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));	// 스피어 컴포넌트 생성
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));	// 충돌 프리셋 설정
	Collision->SetupAttachment(Scene);	// Scene(Root Component)에 붙임

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));	// 스태틱 메시 컴포넌트 생성
	StaticMesh->SetupAttachment(Collision);	// Collision에 붙임

	// 이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);	// 겹쳤을 때 호출되는 함수
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);	// 겹침이 끝났을 때 호출되는 함수
}

void ABaseItem::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Overlap!!!")));
		ActivateItem(OtherActor);  // 플레이어와 겹쳤을 때 아이템 활성화 함수 호출
	}
}

void ABaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABaseItem::ActivateItem(AActor* Activator)
{
	// 파티클 시스템 컴포넌트를 약한 참조로 선언
	// UParticleSystemComponent는 파티클을 제어할 수 있는 컴포넌트
	// TWeakObjectPtr은 UObject가 파괴되면 자동으로 nullptr로 바뀌는 안전한 스마트 포인터
	TWeakObjectPtr<UParticleSystemComponent> Particle = nullptr;

	// 아이템 획득 시 파티클 재생
	if (PickupParticle)
	{
		// 지정된 위치와 회전에서 파티클 생성
		// AutoDestroy = true: 파티클 재생이 끝나면 자동으로 제거됨
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),				// 파티클을 생성할 월드
			PickupParticle,			// 사용할 파티클 에셋
			GetActorLocation(),		// 이 아이템의 현재 위치
			GetActorRotation(),		// 이 아이템의 현재 회전값
			true					// AutoDestroy: 자동 파괴
		);
	}

	// 아이템 획득 시 사운드 재생
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}

	// 파티클이 생성되었다면 수동으로 제거 타이머도 설정 (예비 안전 장치)
	if (Particle.Get())
	{
		FTimerHandle DestroyParticleTimerHandle;

		// 일정 시간 후 파티클 컴포넌트 제거
		// 람다 안에서 다시 IsValid 검사하여 이미 파괴된 경우에도 안전하게 처리
		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[Particle]()
			{
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

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}