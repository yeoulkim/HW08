#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "EnhancedInputSubsystems.h"	// Enhanced Input Subsystem을 사용하기 위한 헤더
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"	// MainMenu Text 관련

ASpartaPlayerController::ASpartaPlayerController()	// 생성자 정의
	: InputMappingContext(nullptr), 
	  MoveAction(nullptr), 
	  JumpAction(nullptr), 
	  LookAction(nullptr), 
	  SprintAction(nullptr),	// BP에서 할당 할 거니까 여기선 nullptr로 초기화
	  HUDWidgetClass(nullptr), 
	  HUDWidgetInstance(nullptr),
	  MainMenuWidgetClass(nullptr),
	  MainMenuWidgetInstance(nullptr)
{	
}

// IMC 활성화
void ASpartaPlayerController::BeginPlay()	// 플레이 시작 시 호출되는 함수
{
	Super::BeginPlay();	// 부모 클래스의 BeginPlay 호출
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())	// 로컬 플레이어 정보 가져오기
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())	// EnhancedInputSubsystem을 관리하는 Subsystem 획득하기
		{
			if (InputMappingContext)	// IMC가 할당되어 있는지 확인
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);	// 획득한 Subsystem에 IMC를 추가, 0은 최우선순위로 두고 활성화 시킴
			}
		}
	}

	// 현재 맵이 mainmap일 경우만 mainmenu 띄우기
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}

	/*
	if (HUDWidgetClass)
	{
		// HUDWidgetClassf라는 instance 만들어서 HUDWidget에 할당해라!
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)	// 안전코드
		{
			HUDWidgetInstance->AddToViewport();	// 생성된 위젯을 뷰포트에 추가해줘
		}
	}

	// 위젯을 뷰포트에 부른 직후 초기화
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->UpdateHUD();
	}
	*/
}

// get 함수 만들기
UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

// 메뉴 UI 표시
void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();	// 뷰포트에서 떼어내고
		HUDWidgetInstance = nullptr;	// nullptr로 처리
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();	// 메인메뉴 생기면 뷰포트에 연결

			// 뒤에 게임(레벨)이 떠있으면 마우스 움직일 때 캐릭터 시점 조작될 수 있음
			// 막기 위해서 마우스 포커스가 UI로만 가게
			bShowMouseCursor = true;	// 게임에선 마우스 커서 안 보이지만 UI에선 보이게
			SetInputMode(FInputModeUIOnly());	// UI만 사용 가능하게 InputMode 처리
		}

		// 메인메뉴가 Start? Restart?
		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));	// 이 때 GameOver Animation 호출해야 함
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			// 애니메이션 실행
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);	//PlayAnimFunc: 매개 변수 없는 함수는 두 번째 인자에 nullptr로 호출
			}

			// 점수 갖고오기
			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
					));
				}
			}
		}
	}
}

// 게임 HUD 표시
void ASpartaPlayerController::ShowGameHUD()
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;	// 게임중이니까 마우스 커서는 없애기
			SetInputMode(FInputModeGameOnly()); // 인풋모드는 게임에서만 사용해!

			ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
			if (SpartaGameState)
			{
				SpartaGameState->UpdateHUD();	// 업데이트해서 갱신
			}
		}
	}
}

// 게임 시작 - BasicLevel 오픈, GameInstance 데이터 리셋
void ASpartaPlayerController::StartGame()
{
	// GameInstance에 있는 정보들 초기화
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	// 지금은 UI 전용 Level에 있음
	// BasicLevel 오픈해라~
	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);	// 게임 끝나면 꺼~
}