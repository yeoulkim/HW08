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
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);
}

void ASpartaGameState::OnLevelTimeUp()
{
	// 시간이 다 되면 레벨을 종료
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount)

		// 모든 코인 수집하면 레벨 종료
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			EndLevel();
		}
}

void ASpartaGameState::EndLevel()
{
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
			AddScore(Score);	// 레벨이 끝날 때마다 점수 더해줌
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
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				// UI 요소들 가져오기
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					// 남은 시간 표시
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);	// 타이머가 몇 초 남았나?
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));	// String -> Text 변환해주는 작업
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					// GameInstance에 TotalScore 있음
					if (UGameInstance* GameInstance = GetGameInstance())	
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					// 현재 레벨 표시
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}