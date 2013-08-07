#include "CBone.h"

CBone::CBone(void) :
	m_gameNode(nullptr),
	m_gameNodeId(-1),
	m_id(-1),
	m_parentId(-1),
	m_parent(nullptr)
{
	
}

CBone::~CBone(void)
{

}

void CBone::LinkBone(CBone* _bone)
{
	_bone->m_parent = _bone;
	_bone->m_parentId = m_id;
	m_childs.push_back(_bone);
}