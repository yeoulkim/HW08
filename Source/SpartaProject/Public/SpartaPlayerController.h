#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

// 미리 선언
class UInputMappingContext;
class UInputAction;

UCLASS()
class SPARTAPROJECT_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();	// 생성자

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")	//에디터에서 수정 가능하도록 설정
	UInputMappingContext* InputMappingContext;	// IMC를 할당해줄 ptr 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* HUDWidgetInstance;

	// 메뉴 UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

	// HUDWidget을 GameState로 넘겨주기
	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	// HUD창 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	// 메인 메뉴창 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);	// 처음: Start, GameOver: Restart - bool 변수
	// MainMenu의 StartButton - 게임 시작 함수
	// Start & Restart - 거의 똑같음 -> StartGame으로 묶기
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

protected:	// IMC 활성화 및 입력 액션 바인딩을 위한 함수들
	virtual void BeginPlay() override;	// 플레이 시작 시 호출되는 함수
	// PlayerController가 생성된 직후에 IMC를 활성화
};
