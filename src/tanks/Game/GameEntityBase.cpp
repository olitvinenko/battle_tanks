#include "GameEntityBase.h"

int GameEntityBase::m_iNextValidID = 0;

GameEntityBase::GameEntityBase(int ID)
	: m_iType(default_entity_type)
	, m_bTag(false)
	, m_vScale(Vector2D(1.0, 1.0))
	, m_dBoundingRadius(0.0)
{
	SetID(ID);
}

void GameEntityBase::SetID(int val)
{
	//make sure the val is equal to or greater than the next available ID
	assert((val >= m_iNextValidID) && "<BaseGameEntity::SetID>: invalid ID");

	m_ID = val;
	m_iNextValidID = m_ID + 1;
}
