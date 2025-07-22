#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

// Component 미리 선언
class UBoxComponent;

UCLASS()
class SPARTAPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpawnVolume();

	// Component 만들기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")	// 객체 바꿔야하니까 EditAnywhere
	UDataTable* ItemDataTable;

	// 테스트 할 수 있으니까 에디터에 노출
	// 랜덤 아이템을 스폰시키는 함수
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	// 에디터에 굳이 노출 안 해도 됨
	// SpawnVolume 안에서 랜덤 좌표 얻는 함수
	FVector GetRandomPointInVolume() const;	
	// 지정된 class의 아이템을 스폰하는 함수
	// 아이템 랜덤으로 갖고 오는 함수
	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);	// 이 액터의 하위클래스가 아니면 무조건 오류남

};