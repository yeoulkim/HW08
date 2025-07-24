#include "CoinItem.h"
#include "Engine/World.h"	// GameState를 가져오기 위해 World 먼저 갖고 오기
#include "SpartaGameState.h"

ACoinItem::ACoinItem()  // 생성자
{
	PointValue = 0;  // 초기화, 부모클래스라서 0점, 자손클래스에서 본인에 해당하는 변수로 값을 넘겨 받음 <다형성>
	ItemType = "DefaultCoin";  // 아이템 타입 초기화
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);	// 점수 올림
				GameState->OnCoinCollected();	// 현재 레벨에서 먹은 코인 개수 알려줌

			}
		}
		DestroyItem();
	}
}

void ACoinItem::SetMove(bool bMove)
{
	if (bMove)
	{
		StartMoving();  // 기본적으로 이동 시작
	}
	else
	{
		StopMoving();   // 기본적으로 이동 멈춤
	}
}

void ACoinItem::StartMoving()
{
	// 기본적으로 이동을 시작하는 로직
	bIsMoving = true;
	SetActorEnableCollision(true);  // 이동을 시작하려면 충돌 활성화
	// 추가적인 이동 로직을 이곳에 넣을 수 있음 
}

void ACoinItem::StopMoving()
{
	// 기본적으로 이동을 멈추는 로직
	bIsMoving = false;
	SetActorEnableCollision(false);  // 이동을 멈추려면 충돌 비활성화
	// 추가적인 이동 로직을 이곳에 넣을 수 있음 
}