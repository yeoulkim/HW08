#include "SmallCoinItem.h"

ASmallCoinItem::ASmallCoinItem()
{
	PointValue = 10;
	ItemType = "SmallCoin";
}

void ASmallCoinItem::ActivateItem(AActor* Activator)
{
	// DestroyItem();
	Super::ActivateItem(Activator);  // 부모 클래스의 ActivateItem 호출
}