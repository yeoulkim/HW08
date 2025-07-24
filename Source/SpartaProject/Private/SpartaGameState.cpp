#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GamePlayStatics.h"
#include "Components/TextBlock.h"	// UI Text 가져오기
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	// 전체 초기화
	// coin 관련 카운트 초기화 하는 변수
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWaveIndex = 0;
	MaxWaves = 3;
	WaveDuration = 20.0f;
	ItemSpawnPerWave = TArray<int32>();	// TArray 비어있는 배열로 초기화
	CurrentWaveItems = TArray<AActor*>();
	bLevelEnded = false;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	// UpdateHUD();
	StartLevel();	// 첫 번째 레벨 시작

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,	// 이 객체에서
		&ASpartaGameState::UpdateHUD,	// 시간이 끝나면 이 함수 불러~
		0.1f,	// 0.1 초마다 UpdateHUD 불러
		true	// 반복
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	// GameInstance 호출
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);	// 전체 총점에 점수 추가
		}	
	}
}

void ASpartaGameState::StartLevel()
{
	// 레벨 오픈 -> 메뉴UI (x) / HUD UI (o)
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();	
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;	// 임시 저장
		}
	}

	// Wave 함수 호출
	CurrentWaveIndex = 0;
	StartWave();
	
	// 레벨을 불러올 때마다 초기화
	// coin 관련 카운트 초기화 하는 변수
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// World에 SpawnVolume이 몇 개 있는지 세고 거기에 아이템 40개 스폰함
	// 찾은 Volume을 배열 형태로 저장
	TArray<AActor*> FoundVolumes;
	// 현재 월드에서 Actor에 해당되는 모든 걸 가져옴 -> 찾은 것들을 FoundVolumes에 넣음
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	// 찾은 볼륨들에 몇 개를 스폰할지 지정
	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)	// 0개 이상인지 확인
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();	// 9.1.b에서 void 바꿔놨음
				// IsA: Coin 맞음??, 하위클래스까지 인식
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))	
				{
					SpawnedCoinCount++;

				}
			}
		}

	}

	// 30초 후에 OnLevelTimeUp()가 호출되도록 타이머 설정
/*
 GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
		); */
}

// Wave에서 SpawnVolume 쓰니까 GetSpawnVolume 함수 만들어줘야함
// 현재 월드에서 ASpawnVolume 객체를 찾아서 그 객체를 바나환하는 함수
ASpawnVolume* ASpartaGameState::GetSpawnVolume()
{
	// SpawnVolume을 찾을 TArray 생성
	// ASpawnVolume 클래스는 AActor를 상속받고 있으니까 이 배열은 AActor* 타입으로 정의
	TArray<AActor*> FoundVolumes;	// FoundVolumes는 AActor 객체들을 담을 수 있는 배열

	// 현재 월드에서 SpawnVolume 클래스를 가진 모든 액터를 찾아 FoundVolumes에 저장
	// 첫 번째 인자: 현재 월드를 반환 -> 게임 내에 존재하는 환경, 레벨, 맵
	// 두 번째 인자: ASpawnVolume 클래스를 나타내는 UClass 객체
	// -> StaticClass() 함수는 클래스 자체를 반환하고 이 클래스를 가진 액터들을 찾아야 하기 때문에 사용
	// StaticClass는 리플렉션 위해 사용!
	// 세 번째 인자: FoundVolumes는 ASpawnVolume 클래스를 가진 액터들을 담을 배열 <- 여기에 찾은 액터들이 추가됨
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	// SpawnVolume이 하나라도 있으면 첫 번째 SpawnVolume 반환
	if (FoundVolumes.Num() > 0)	// 월드에 ASpawnVolume이 하나라도 존재한다면
	{
		// FoundVolumes[0]은 AActor* 타입인데 SpawnVolume* 타입으로 변환
		// ASpawnVolume은 AActor를 상속받는 클래스이기 때문에 캐스팅이 가능
		return Cast<ASpawnVolume>(FoundVolumes[0]);	// 첫 번째 SpawnVolume을 반환
	}

	// FoundVolumes 배열에 ASpawnVolume 액터가 하나도 없다면
	// nullptr은 "없다" -> ASpawnVolume 객체를 찾지 못했다!!
	return nullptr;	
}

// Wave 함수
void ASpartaGameState::StartWave()
{
	// 먹은 코인 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 이전 Wave 아이템 없애기
	for (AActor* Item : CurrentWaveItems)
	{
		if (Item && Item->IsValidLowLevelFast())	// IsValid는 시한부, 나중에 없앨 애들
		{
			Item->Destroy();
		}
	}
	CurrentWaveItems.Empty();	// Item 확실하게 없애기

	// 이번 Wave에 스폰할 아이템 개수 정하기
	// Index 유효하면 그 Index 요소 받아오기 유효하지 않으면 20개
	int32 ItemToSpawn = (ItemSpawnPerWave.IsValidIndex(CurrentWaveIndex)) ? ItemSpawnPerWave[CurrentWaveIndex] : 20;

	// 아이템 스폰하기
	if (ASpawnVolume* SpawnVolume = GetSpawnVolume())
	{
		for (int32 i = 0; i < ItemToSpawn; i++)
		{
			if (AActor* SpawnedActor = SpawnVolume->SpawnRandomItem())
			{
				// SpawnedActor가 Coin 아이템인지 확인
				if (SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					// Coin 카운트 증가
					SpawnedCoinCount++;
				}

				// Spawn된 Actor를 CurrentWaveItems 배열에 추가
				CurrentWaveItems.Add(SpawnedActor);
			}
		}
	}
	
	// Wave별 환경 변화
	if (CurrentWaveIndex == 1)
	{
		EnableWave2();	// 인덱스가 1이면 2번째 웨이브
	}
	else if (CurrentWaveIndex == 2)
	{
		EnableWave3();	// 인덱스가 2이면 3번째 웨이브
	}
	
	UpdateHUD();	// 다음 Wave로 나오게 UI 업데이트 해!!
	
	// Wave 타이머 시작 (WaveDuration 후에 OnWaveTimeUp 호출)
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);
}

void ASpartaGameState::EnableWave2()
{
	// Wave 2에 대한 설정 작업을 여기에 작성
}

void ASpartaGameState::EnableWave3()
{
	// Wave 3에 대한 설정 작업을 여기에 작성
}

void ASpartaGameState::OnLevelTimeUp()
{
	// 레벨 시간이 다 되면 레벨 종료
	EndLevel();
}

void ASpartaGameState::EndWave()
{
	// 남은 Wave 타이머 제거
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	// Wave3에서 활성화한 코인 궤도, 회전 비활성화
	if (CurrentWaveIndex == 2)
	{
		SetAllCoinsMove(false);
	}

	// 웨이브 인덱스 증가는 여기서 한 번만
	++CurrentWaveIndex;

	// 다음 Wave 진행 또는 Level 종료
	if (CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		StartWave();
	}
}

// 모든 코인의 이동을 제어하는 함수
// bool Move -> 게임 내 모든 코인을 제어하고, 이동을 활성화 or 비활성화
void ASpartaGameState::SetAllCoinsMove(bool bMove)
{
	// AllCoins 배열에 있는 모든 코인 아이템에 대해 처리
	for (ACoinItem* Coin : AllCoins)
	{
		if (Coin)
		{
			// 코인 아이템이 존재한다면, 이동을 정지
			Coin->SetMove(bMove);  // SetMove는 코인 아이템에서 이동 여부를 설정하는 함수일 것
		}
	} 
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount);

	// 모든 코인 수집하면 웨이브 종료
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		// 웨이브 종료
		EndWave();
	}

	// 모든 코인 수집 완료 후 레벨 종료
	if (CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void ASpartaGameState::OnWaveTimeUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Wave Time Up!"));

	EndWave();  // 현재 웨이브 정리

	CurrentWaveIndex++;

	if (CurrentWaveIndex < MaxWaves)
	{
		StartWave();  // 다음 웨이브 시작
	}
	else
	{
		OnLevelTimeUp();  // 마지막 웨이브였으면 레벨 종료
	}
}

void ASpartaGameState::EndLevel()
{
	// ✅ 중복 호출 방지
	if (bLevelEnded)
	{
		return;
	}
	bLevelEnded = true;

	// 타이머 해제
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// 인덱스 추가해서 다음 레벨로~
	CurrentLevelIndex++;

	// 점수 추가
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);	
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	// 모든 레벨을 다 돌았다면 게임 오버 처리
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	// 다음 레벨 불러오기
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	// 게임 오버 후 UI 및 게임 정지
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);	// 게임이 완전히 정지될 수 있게 게임 자체를 멈추기
			SpartaPlayerController->ShowMainMenu(true);	// Restart bool : true
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	// 현재 플레이어 컨트롤러 가져오기
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		// Sparta 전용 컨트롤러로 캐스팅
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		if (SpartaPlayerController)
		{
			// Wave 타이머 남은 시간 (타이머가 돌고 있을 때만)
			float WaveRemainingTime = 0.0f;
			if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
			{
				WaveRemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
			}

			// Wave 정보 UI에 업데이트 (Wave: 1 / 3 같은 텍스트)
			SpartaPlayerController->UpdateWaveUI(CurrentWaveIndex, MaxWaves, WaveRemainingTime);
			

			// UD 위젯 가져오기
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// [Time] 텍스트 UI 갱신
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), WaveRemainingTime)));
				}

				// [Score] 텍스트 UI 갱신
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					// GameInstance에서 점수 가져오기
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				// [Level] 텍스트 UI 갱신
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}

