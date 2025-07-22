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
	// 지정된 위치랑 회전에서 파티클 재생해 ~
	if (PickupParticle)
	{
		// 지정된 위치나 회전에 따라 파티클 효과 생성하는 함수
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),	// 현재 게임이 실행되고 있는 월드 객체 가져옴<-파티클 생성한 월드 정보 필요
			PickupParticle,	// 지정해놓은 파티클 에셋
			GetActorLocation(),	// 이 아이템의 월드 위치 가져옴
			GetActorRotation(),	// 이 아이템의 회전 위치 가져옴
			true	// AutoDistroy : 파티클 효과 끝난 이후에 메모리에서 자동 제거 되도록 설정
		);
	}

	// 지정된 위치에서 사운드 효과 재생해 ~
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
		);
	}

	UParticleSystemComponent* Particle = nullptr;  // 초기화 필요
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

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
}