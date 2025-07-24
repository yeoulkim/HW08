#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CoinItem.h"
#include "SpartaGameState.generated.h"

// ASpartaGameStateComponent -> 블록

// SpawnVolum 함수 cpp에서 쓰니까 전받선언 해주기
class ASpawnVolume;

UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameStateBase
{
	GENERATED_BODY()


public:
	ASpartaGameState();	// 생성자

	// 게임 시작 직후 레벨 스타트 하기 위해
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coins")
	TArray<ACoinItem*> AllCoins;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FName> LevelMapNames;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevel;
	

	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle WaveTimerHandle;	// Wave 타이머 설정
	

	// Wave 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<int32> ItemSpawnPerWave;	// wave마다 item  개수 다르게
	TArray<AActor*> CurrentWaveItems;	// Wave 아이템을 저장하는 배열
	// WaveData.Num() == 100
	// 레벨 웨이브 100
    // TArray<WaveData> WaveData;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
	UFUNCTION(BlueprintPure, Category = "SpawnVolume")
	ASpawnVolume* GetSpawnVolume();	// spawnvolume을 반환하는 함수
	UFUNCTION()
	void OnWaveTimeUp();

	void StartLevel();
	void StartWave();
	// Wave1은 CurrentWaveIndex == 0인 경우 첫 번째 웨이브가 시작되는 것
	// 그냥 기본적으로 시작되니까 정의 안 해도 됨!
	void EnableWave2();
	void EnableWave3();
	// 이동 제어 함수
	void SetAllCoinsMove(bool bMove);
	void OnLevelTimeUp();
	// 코인을 주웠을 때 호출
	void OnCoinCollected();
	void EndLevel();
	void EndWave();
	void UpdateHUD();
	bool bLevelEnded;
};
