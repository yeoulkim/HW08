#include "HealingItem.h"
#include "SpartaCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f; // 기본 회복량 설정
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	// DestroyItem();
	if (Activator && Activator->ActorHasTag("Player"))
	{
		// Activator가 Actor 클래스이므로 얘를 SpartaCharacter로 캐스팅 해야함
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->AddHealth(HealAmount);
		}
		DestroyItem();
	}
}