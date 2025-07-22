#include "SpartaGameMode.h"
#include "SpartaCharacter.h"	
#include "SpartaPlayerController.h"	 // GameMode에서 PlayerController 관리
#include "SpartaGameState.h"


ASpartaGameMode::ASpartaGameMode()	// 생성자 정의
{
	DefaultPawnClass = ASpartaCharacter::StaticClass();	
	PlayerControllerClass = ASpartaPlayerController::StaticClass();	// PlayerController 설정
	GameStateClass = ASpartaGameState::StaticClass();	// Class를 Static 형태로 넣어주기
}