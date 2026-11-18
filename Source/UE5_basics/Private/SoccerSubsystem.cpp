// Fill out your copyright notice in the Description page of Project Settings.

#include "SoccerSubsystem.h"
#include "TagBall.h"
#include "TP_TestCharacter.h"
#include "Other/HitCounter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void USoccerSubsystem::InitializeMatch()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// 1. Raccoglie tutti i player e tutte le porte presenti
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(World, ATP_TestCharacter::StaticClass(), FoundPlayers);

	TArray<AActor*> FoundGoals;
	UGameplayStatics::GetAllActorsOfClass(World, AHitCounter::StaticClass(), FoundGoals);

	if (FoundPlayers.Num() == 0 && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			4.0f,
			FColor::Red,
			TEXT("SoccerSubsystem: Nessun Player trovato durante InitializeMatch! Chiamare con un piccolo delay dopo lo spawn."));
	}

	if (FoundGoals.Num() == 0 && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			4.0f,
			FColor::Red,
			TEXT("SoccerSubsystem: Nessuna Porta trovata durante InitializeMatch!"));
	}

	const TArray<FLinearColor> TeamColors = {
		FLinearColor(0.05f, 0.25f, 1.0f, 1.0f), // Blu
		FLinearColor(1.0f, 0.05f, 0.05f, 1.0f)  // Rosso
	};

	TArray<AHitCounter*> AvailableGoals;
	for (AActor* Actor : FoundGoals)
	{
		if (AHitCounter* Goal = Cast<AHitCounter>(Actor))
		{
			AvailableGoals.Add(Goal);
		}
	}

	// 2. Associa ogni giocatore alla porta piu' vicina
	for (int32 PlayerIdx = 0; PlayerIdx < FoundPlayers.Num(); ++PlayerIdx)
	{
		ATP_TestCharacter* Player = Cast<ATP_TestCharacter>(FoundPlayers[PlayerIdx]);
		if (!IsValid(Player))
		{
			continue;
		}

		// print on screen player index
		FString PlayerIndexStr = FString::Printf(TEXT("Player index: %d"), PlayerIdx);

		const FLinearColor Color = TeamColors.IsValidIndex(PlayerIdx) ? TeamColors[PlayerIdx] : FLinearColor::White;
		Player->SetTeamColor(Color);
		
		// Trova la porta piu' vicina ancora non assegnata
		AHitCounter* BestGoal = nullptr;
		float MinDistanceSq = TNumericLimits<float>::Max();
		int32 BestGoalIdx = INDEX_NONE;

		for (int32 GoalIdx = 0; GoalIdx < AvailableGoals.Num(); ++GoalIdx)
		{
			AHitCounter* Goal = AvailableGoals[GoalIdx];
			if (IsValid(Goal))
			{
				const float DistSq = FVector::DistSquared(Player->GetActorLocation(), Goal->GetActorLocation());
				if (DistSq < MinDistanceSq)
				{
					MinDistanceSq = DistSq;
					BestGoal = Goal;
					BestGoalIdx = GoalIdx;
				}
			}
		}

		if (IsValid(BestGoal))
		{
			BestGoal->SetGoalOwner(Player, Color);
			AvailableGoals.RemoveAt(BestGoalIdx);
		}
	}
}

void USoccerSubsystem::SetBallSpawnPointActor(AActor* SpawnActor)
{
	BallSpawnPointActor = SpawnActor;
}

void USoccerSubsystem::RegisterGoalFromBall(ATagBall* Ball, AHitCounter* GoalHit, int32 Points)
{
	if (!IsValid(Ball) || !Ball->IsInFlight())
	{
		return;
	}

	// Cattura automaticamente la classe effettiva della palla se non configurata manualmente
	if (!IsValid(BallClass))
	{
		BallClass = Ball->GetClass();
	}

	ATP_TestCharacter* Thrower = Ball->GetLastThrower();
	ATP_TestCharacter* GoalDefender = IsValid(GoalHit) ? GoalHit->GetGoalOwner() : nullptr;

	ATP_TestCharacter* AwardedPlayer = Thrower;

	// Logica di attribuzione:
	// Se chi tira segna nella propria porta (Thrower == GoalDefender), il punto va all'avversario
	if (IsValid(Thrower) && IsValid(GoalDefender) && Thrower == GoalDefender)
	{
		for (const auto& Pair : PlayerScores)
		{
			if (Pair.Key != Thrower)
			{
				AwardedPlayer = Pair.Key;
				break;
			}
		}
	}

	RegisterPlayerGoal(AwardedPlayer, Points);

	// Distrugge la palla attuale e pianifica il respawn
	Ball->Explode();
	ScheduleBallRespawn();
}

void USoccerSubsystem::RegisterPlayerGoal(ATP_TestCharacter* Scorer, int32 Points)
{
	++TotalGoalsCount;
	OnTotalGoalsChanged.Broadcast(TotalGoalsCount);

	int32 NewScore = Points;

	if (IsValid(Scorer))
	{
		if (int32* CurrentScore = PlayerScores.Find(Scorer))
		{
			*CurrentScore += Points;
			NewScore = *CurrentScore;
		}
		else
		{
			PlayerScores.Add(Scorer, Points);
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.5f,
				FColor::Green,
				FString::Printf(TEXT("GOAL! Player: %s | Punteggio: %d | Goal Totali: %d"),
					*Scorer->GetName(), NewScore, TotalGoalsCount));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.5f,
				FColor::Yellow,
				FString::Printf(TEXT("GOAL! Autore sconosciuto | Goal Totali: %d"), TotalGoalsCount));
		}
	}

	OnGoalScored.Broadcast(Scorer, NewScore);
}

void USoccerSubsystem::ScheduleBallRespawn()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&USoccerSubsystem::SpawnNewBall,
			RespawnDelay,
			false);
	}
}

void USoccerSubsystem::SpawnNewBall()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	TSubclassOf<ATagBall> ClassToSpawn = IsValid(BallClass) ? BallClass : TSubclassOf<ATagBall>(ATagBall::StaticClass());
	if (!IsValid(ClassToSpawn))
	{
		return;
	}

	FTransform SpawnTransform = FTransform::Identity;
	if (BallSpawnPointActor.IsValid())
	{
		SpawnTransform = BallSpawnPointActor->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATagBall* NewBall = World->SpawnActor<ATagBall>(ClassToSpawn, SpawnTransform, SpawnParams);
	if (IsValid(NewBall))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Cyan,
				TEXT("Palla rigenerata allo Spawn Point!"));
		}
	}
}

int32 USoccerSubsystem::GetScoreForPlayer(const ATP_TestCharacter* Player) const
{
	if (!IsValid(Player))
	{
		return 0;
	}

	if (const int32* FoundScore = PlayerScores.Find(Player))
	{
		return *FoundScore;
	}

	return 0;
}

void USoccerSubsystem::ResetMatchStats()
{
	PlayerScores.Empty();
	TotalGoalsCount = 0;
	OnTotalGoalsChanged.Broadcast(TotalGoalsCount);
}

