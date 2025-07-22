#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// component 초기화
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;	// 초기화 안 해줘서 안전하게 해줌
}

// 아이템 랜덤으로 갖고 오는 함수
AActor* ASpawnVolume :: SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			// // 여기서 SpawnItem()을 호출하고, 스폰된 AActor 포인터를 리턴
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;	// 실패 시
}

// 랜덤한 행 갖고 오는 함수
FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{	
	// 데이터 테이블이 유효한지 확인
	if (!ItemDataTable) return nullptr;	// 유효하지 않으면 nullptr 반환, 이 행을 반환
	// 테이블에서 모든 행 가져오기 (배열 형태로)
	// 이 모든 FItemSpawnRow의 포인터를 담을 배열 생성
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));	// 데이터 테이블에서 디버깅 역할
	ItemDataTable->GetAllRows(ContextString, AllRows);	// 모든 행을 AllRows에 저장

	// 비어있는 상황이면 nullptr 리턴하자
	if (AllRows.IsEmpty()) return nullptr;

	// 전체 확률의 합 구하기
	float TotalChance = 0.0f;	// 합 초기화
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;	// 값 누적
		}
	}
	
	// 누적 확률 방식의 랜덤 뽑기
	// 0 ~ 총합 사이 랜덤값 뽑기 -> FMath::FRandRange 사용
	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;	// 누적 확률 초기화

	// 누적 확률로 아이템 선택
	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// 랜덤 위치 알기 위해 컴포넌트 크기 계산하기
	// size (200, 100, 50) scale (2, 1, 1) -> (400, 10, 10) 이렇게 적용된 반지름 값을 반환함
	// Extent : 중심 ~ 끝 거리
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();	// BoxComponent 크기 갖고 오기

	// 중심 좌표 = 컴포넌트가 위치한 값
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	// 어떤 랜덤한 위치(X, Y, Z축)에 소환됨
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),	// 두 값 사이의 랜덤한 값
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

// 실질적으로 Spawn해주는 함수
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	// 안전 코드
	if (!ItemClass) return nullptr;

	// SpawnActor가 성공하면 스폰된 액터의 포인터가 반환됨
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),	// 위치 (랜덤 값 갖고 옴)
		FRotator::ZeroRotator	// 회전은 안하게
	);

	return SpawnedActor;
}
