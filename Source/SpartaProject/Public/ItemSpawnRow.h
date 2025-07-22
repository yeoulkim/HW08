#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.generated.h"

// 구조체 선언
USTRUCT(BlueprintType)   // 블루프린트에서 사용 가능 (이 구조체를 변수로 만듦)
struct FItemSpawnRow : public FTableRowBase   // 데이터 테이블의 행으로 얘 사용
{
    GENERATED_BODY()

public:
    // 행 하나에 세 가지 정보
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemName;   // 이름은 가볍게 FName
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ItemClass;   // 아이템 클래스 가져오기
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnChance;   // 아이템 확률
};
