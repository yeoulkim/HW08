#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"  
#include "SpartaCharacter.generated.h"

// 헤더에서 include를 사용하지 않고 미리 선언을 사용
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;	// 구조체

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")	// 객체 변경은 불가능, 내부 속성은 에디터에서 조정 가능
	USpringArmComponent* SpringArmComp;	// 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// UI 붙이기 - Widget BP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// 체력을 갖고 오는 함수
	UFUNCTION(Blueprintpure, Category = "Health")
	float GetHealth() const;

	// 얼마 회복 받을지
	UFUNCTION(BlueprintCallable, Category = "Heal")
	void AddHealth(float Amount);

protected:
	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	// IA에다 동작 함수들 연결하는 장소
	virtual float TakeDamage(
		float DamageAmount,	// 데미지 얼마나 입었나
		struct FDamageEvent const& DamageEvent,	// 데미지 유형 - 스킬에 따라 어떤 데미지?
		AController* EventInstigator,	// 데미지 누가 입힘? (지뢰를 심은 사람)
		AActor* DamageCasuer) override;	// 데미지를 일으킨 오브젝트 (지뢰)

	// 함수 선언
	// IA 만들 때 설정한 valuetype
	// 구조체 같은 건 크기 때문에 참조 안 하고 갖고 오면 객체의 모든 데이터를 복사해서 갖고 옴 -> 성능, 복사 비용 크다
	// 수정 못하게 const로 선언
	// 리플렉션 시스템에 등록만
	UFUNCTION()
	void Move(const FInputActionValue& value);	
	UFUNCTION()
	// boolean 타입은 on/off 상태를 나눠주는게 좋음
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value); 
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverheadHP();

private:
	float NormalSpeed;	// 기본 속도
	float SprintSpeedMultiplier;	// 기본 속도에 몇 개를 곱해줄건데
	float SprintSpeed;	// 위에 두 개를 곱해서 나오는 속도 = 얼마나 빨라졌는지
};
