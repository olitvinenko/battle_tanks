#include "Deathmatch.h"
#include "GameEvents.h"
#include "WorldController.h"
#include "gc/GameClasses.h"
#include "gc/Player.h"
#include "gc/Vehicle.h"
#include "gc/World.h"
#include "gc/SaveFile.h"

Deathmatch::Deathmatch(World &world, WorldController &worldController, GameListener &gameListener)
	: _world(world)
	, _worldController(worldController)
	, _gameListener(gameListener)
{
	_world.eGC_RigidBodyStatic.AddListener(*this);
}

Deathmatch::~Deathmatch()
{
	_world.eGC_RigidBodyStatic.RemoveListener(*this);
}

void Deathmatch::Step()
{
//	if( IsGameOver() )
//	{
//		for( auto ls: eWorld._listeners )
//			ls->OnGameFinished();
//	}
}

int Deathmatch::GetRating() const
{
	int maxRating = 0;

	if (IsGameOver())
	{
		for (auto player : _worldController.GetLocalPlayers())
		{
			if (player->GetScore() == _maxScore)
			{
				int rating = 1;
				rating += (player->GetNumDeaths() == 0);
				rating += (_world.GetTime() < _timeLimit);
				maxRating = std::max(maxRating, rating);
			}
		}
	}

	return maxRating;
}

bool Deathmatch::IsGameOver() const
{
	return (_timeLimit > 0 && _world.GetTime() >= _timeLimit) ||
		(_fragLimit > 0 && _maxScore >= _fragLimit);
}

void Deathmatch::Serialize(SaveFile &f)
{
	f.Serialize(_fragLimit);
	f.Serialize(_timeLimit);
}

void Deathmatch::OnDestroy(GC_RigidBodyStatic &obj, const DamageDesc &dd)
{
	if (auto vehicle = dynamic_cast<GC_Vehicle*>(&obj))
	{
		char score[8] = { 0 };
		GC_Text::Style style = GC_Text::DEFAULT;

		if( dd.from )
		{
			if( dd.from == vehicle->GetOwner() )
			{
				vehicle->GetOwner()->SetScore(vehicle->GetOwner()->GetScore() - 1);
				style = GC_Text::SCORE_MINUS;
				_gameListener.OnMurder(*vehicle->GetOwner(), vehicle->GetOwner(), MurderType::Suicide);
			}
			else if( vehicle->GetOwner() )
			{
				if( 0 != vehicle->GetOwner()->GetTeam() &&
				    dd.from->GetTeam() == vehicle->GetOwner()->GetTeam() )
				{
					dd.from->SetScore(dd.from->GetScore() - 1);
					style = GC_Text::SCORE_MINUS;
					_gameListener.OnMurder(*vehicle->GetOwner(), dd.from, MurderType::Friend);
				}
				else
				{
					// 'from' killed his enemy
					dd.from->SetScore(dd.from->GetScore() + 1);
					style = GC_Text::SCORE_PLUS;
					_gameListener.OnMurder(*vehicle->GetOwner(), dd.from, MurderType::Enemy);
				}
			}
			else
			{
				// this tank does not have player. score up the killer
				dd.from->SetScore(dd.from->GetScore() + 1);
				style = GC_Text::SCORE_PLUS;
			}

			if( dd.from->GetVehicle() )
			{
				snprintf(score, sizeof(score), "%d", dd.from->GetScore());
				_world.New<GC_Text_ToolTip>(dd.from->GetVehicle()->GetPos(), score, style);
			}
		}
		else if( vehicle->GetOwner() )
		{
			vehicle->GetOwner()->SetScore(vehicle->GetOwner()->GetScore() - 1);
			_gameListener.OnMurder(*vehicle->GetOwner(), nullptr, MurderType::Accident);

			snprintf(score, sizeof(score), "%d", vehicle->GetOwner()->GetScore());
			_world.New<GC_Text_ToolTip>(vehicle->GetPos(), score, GC_Text::SCORE_MINUS);
		}

		int maxScore = INT_MIN;
		for (auto player: _worldController.GetLocalPlayers())
			maxScore = std::max(maxScore, player->GetScore());
		for (auto player : _worldController.GetAIPlayers())
			maxScore = std::max(maxScore, player->GetScore());
		_maxScore = maxScore;
	}
}
