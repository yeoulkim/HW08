#include "SpartaCharacter.h"
#include "EnhancedInputComponent.h"	// Enhanced Input 시스템을 사용하기 위한 include
#include "SpartaPlayerController.h"	// 플레이어 컨트롤러를 사용하기 위한 include
// 헤더 파일에서 include를 사용하지 않고 미리 선언을 사용
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"  
#include "Components/TextBlock.h"  
#include "GameFramework/SpringArmComponent.h"
#include "SpartaGameState.h"
#include "CoinItem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{ 
	PrimaryActorTick.bCanEverTick = false;	// 이 캐릭터는 Tick을 사용하지 않음

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp")); // 컴포넌트 생성
	SpringArmComp->SetupAttachment(RootComponent);	// 루트 컴포넌트에 연결
	SpringArmComp->TargetArmLength = 300.0f; // 스프링 암(삼각대)의 길이 설정
	SpringArmComp->bUsePawnControlRotation = true; // 캐릭터의 회전을 스프링 암이 따르도록 설정

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // 스프링 암 끝 부분에 연결
	CameraComp->bUsePawnControlRotation = false; // 카메라가 캐릭터의 회전을 따르지 않도록 설정

	// Widget 붙이기
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());	// 코드에는 Mesh 없으니까 가져오기
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);	// screen 모드

	NormalSpeed = 600.0f;	// 기본 속도 초기화
	SprintSpeedMultiplier = 1.7f;	// 스프린트 속도 배율 초기화
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;	

	// CharacterMovement 컴포넌트가 여러 이동 함수들을 가지고 있음
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;	// 캐릭터의 최대 걷기 속도를 기본 속도로 설정

	// 체력 변수들 초기화
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))	// PlayerInputComponent 여러 갠데 우리가 사용하는 EnhancedInput 기능으로 정해주기
	{
		ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController());	// 현재 캐릭터가 조작하는 Controller get -> SpartaPlayerController로 한 번 캐스팅

		if (PlayerController == nullptr)	// null 체크
		{
			return;
		}

		if (PlayerController->MoveAction)	// IMC가 할당되어 있다면
		{
			EnhancedInput->BindAction(	// BindAction : 이벤트랑 함수를 연결하는 핵심 코드
				PlayerController->MoveAction,	// IA 가져오기
				ETriggerEvent::Triggered,	// IMC에서 설정한 트리거 이벤트
				this,	// 이 캐릭터에 바인딩
				&ASpartaCharacter::Move	// Move 함수에 바인딩, 포인터 - 호출된 함수 주소 가져옴
			);
		}

		if (PlayerController->JumpAction)
		{
			EnhancedInput->BindAction(
				PlayerController->JumpAction,
				ETriggerEvent::Triggered,
				this,
				&ASpartaCharacter::StartJump
			);

			EnhancedInput->BindAction(
				PlayerController->JumpAction,
				ETriggerEvent::Triggered,
				this,
				&ASpartaCharacter::StopJump
			);
		}

		if (PlayerController->LookAction)
		{
			EnhancedInput->BindAction(
				PlayerController->LookAction,
				ETriggerEvent::Triggered,
				this,
				&ASpartaCharacter::Look
			);
		}

		if (PlayerController->SprintAction)
		{
			EnhancedInput->BindAction(
				PlayerController->SprintAction,
				ETriggerEvent::Started,
				this,
				&ASpartaCharacter::StartSprint
			);

			EnhancedInput->BindAction(
				PlayerController->SprintAction,
				ETriggerEvent::Completed,
				this,
				&ASpartaCharacter::StopSprint
			);
		}
	}
}
	
void ASpartaCharacter::Move(const FInputActionValue& value)
{
	// 컨트롤러가 있어야 방향 계산이 가능
	if (!Controller) return;

	// Value는 Axis2D로 설정된 IA_Move의 입력값 (WASD)을 담고 있음
	// 예) (X=1, Y=0) → 전진 / (X=-1, Y=0) → 후진 / (X=0, Y=1) → 오른쪽 / (X=0, Y=-1) → 왼쪽
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		// 캐릭터가 바라보는 방향(정면)으로 X축 이동
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		// 캐릭터의 오른쪽 방향으로 Y축 이동
		AddMovementInput(GetActorRightVector(), MoveInput.Y );
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue & value)
{
	if (value.Get<bool>())	// bool 값이 true라면
	{

		Jump();	// 점프 함수 호출
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue & value)
{
	if (!value.Get<bool>())	// bool 값이 false라면
	{
		StopJumping();	// 점프 중지 함수 호출
	}
}

void ASpartaCharacter::Look(const FInputActionValue & value)
{
	FVector2D LookInput = value.Get<FVector2D>();	// 입력값을 2D 벡터로 가져옴

	AddControllerYawInput(LookInput.X);	// X축 입력값을 yaw 회전에 적용
	AddControllerPitchInput(LookInput.Y);	// Y축 입력값을 pitch 회전에 적용
}

void ASpartaCharacter::StartSprint(const FInputActionValue & value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;	// 캐릭터의 최대 걷기 속도를 스프린트 속도로 설정
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue & value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;	// 캐릭터의 최대 걷기 속도를 기본 속도로 설정
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCasuer)
{
	// 부모 클래스 거 호출
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCasuer);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();	// 사망 
	}
	
	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}