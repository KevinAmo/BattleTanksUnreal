// Copyright Kevin Amores 2018

#include "Public/TankAIController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Public/TankAimingComponent.h"
#include "Public/Tank.h" // So we can implement OnDeath
// Depends on movement component via pathfinding system

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
	UTankAimingComponent* AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();

	if ((PlayerTank) && (AimingComponent))
	{
		// Move towards the players
		MoveToActor(PlayerTank, AcceptanceRadius);

		// Aim towards the player
		AimingComponent->AimAt(PlayerTank->GetActorLocation());

		if(AimingComponent->GetFiringState() == EFiringState::Locked)
		{
			AimingComponent->Fire(); // TODO limit firing rate
		}
	}
}

void ATankAIController::SetPawn(APawn * InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }
		
		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);
	}
}

void ATankAIController::OnPossessedTankDeath()
{
	if (!GetPawn()) { return; }
	GetPawn()->DetachFromControllerPendingDestroy();
}
