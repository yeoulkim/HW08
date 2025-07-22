#include "BigCoinItem.h"

ABigCoinItem::ABigCoinItem()
{
	PointValue = 50;
	ItemType = "BigCoin";
}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
	// DestroyItem(); -> CoinItem에서 DestroyItem()을 호출 
	Super::ActivateItem(Activator);  // 부모 클래스의 ActivateItem 호출
}