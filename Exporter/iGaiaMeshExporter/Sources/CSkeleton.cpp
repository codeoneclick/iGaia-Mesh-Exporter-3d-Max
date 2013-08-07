#include "CSkeleton.h"
#include "CBone.h"

CSkeleton::CSkeleton(IGameObject* _gameObject, IGameNode* _gameNode) :
	m_gameObject(_gameObject),
	m_gameNode(m_gameNode)
{

}

CSkeleton::~CSkeleton(void)
{

}

void CSkeleton::_LinkBone(IGameNode* _gameNode, i32 _gameNodeId)
{
	if (nullptr != CSkeleton::_Get_Bone(_gameNodeId))
	{
		return;
	}

	CBone* bone = new CBone();
	bone->Set_GameNode(_gameNode);
	bone->Set_GameNodeId(_gameNodeId);
	bone->Set_Name(_gameNode->GetName());
	bone->Set_Id(m_bones.size());
	m_bones.push_back(bone);
}


void CSkeleton::_LinkBone(CBone* bone)
{
	IGameNode* gameNode = bone->Get_GameNode();
	IGameNode* gameNodeParent = gameNode->GetNodeParent();
	CBone* parentBone = nullptr;

	if (gameNodeParent != nullptr)
	{
		parentBone = CSkeleton::_Get_Bone(gameNodeParent->GetNodeID());
	}

	if (parentBone == nullptr)
	{
		m_roots.push_back(bone);
	}
	else
	{
		parentBone->LinkBone(bone);
	}
}

CBone* CSkeleton::_Get_Bone(i32 _gameNodeId)
{
	std::vector<CBone*>::const_iterator iterator = m_bones.begin();
	for(; iterator != m_bones.end(); ++iterator)
	{
		if ((*iterator)->Get_GameNodeId() == _gameNodeId)
		{
			return (*iterator);
		}
	}
	return nullptr;
}

i32  CSkeleton::Get_BoneId(i32 _gameNodeId)
{
	CBone* bone = _Get_Bone(_gameNodeId);
	if (bone == nullptr)
	{
			return -1;
	}
	return bone->Get_Id();
};

bool CSkeleton::Bind(void)
{
	if(m_gameObject == nullptr)
	{
		return false;
	}
	
	IGameSkin* skin = m_gameObject->GetIGameSkin();
	if(!skin)
	{
		return false;
	}

	if(IGameSkin::IGAME_SKIN != skin->GetSkinType())
	{
		return false;
	}

	i32 vertexType;
	const i32 numSkinnedVerts = skin->GetNumOfSkinnedVerts();

	for(i32 i = 0; i < numSkinnedVerts; ++i)
	{
		vertexType = skin->GetVertexType(i);
		if(IGameSkin::IGAME_RIGID == vertexType)
		{
			CSkeleton::_LinkBone(skin->GetIGameBone(i, 0), skin->GetBoneID(i, 0));
		}
		else
		{
			for(int j = 0; j < skin->GetNumberOfBones(i); ++j )
			{
				CSkeleton::_LinkBone(skin->GetIGameBone(i, j), skin->GetBoneID(i, j));
			}
		}
	}

	std::vector<CBone*>::const_iterator iterator = m_bones.begin();
	for (; iterator != m_bones.end(); ++iterator)
	{
		CSkeleton::_LinkBone((*iterator));
	}
	return true;
}

void CSkeleton::Serialize(std::ofstream& _stream)
{
	i32 numBones = m_bones.size();
	_stream.write((char*)&numBones, sizeof(i32));

	std::vector<CBone*>::iterator iterator = m_roots.begin();
	for(; iterator != m_roots.end(); ++iterator)
	{
		CBone* bone = (*iterator);
		CSkeleton::_SerializeBone(_stream, bone);
	}
}

void CSkeleton::_SerializeBone(std::ofstream& _stream, CBone* bone)
{
	i32 boneId = bone->Get_Id();
	i32 boneParentId = bone->Get_ParentId();
	_stream.write((char*)&boneId, sizeof(i32));
	_stream.write((char*)&boneParentId, sizeof(i32));

	std::vector<CBone*> childs = bone->Get_Childs();
	std::vector<CBone*>::iterator iterator = childs.begin();
	for(; iterator != childs.end(); ++iterator)
	{
		CBone* bone = (*iterator);
		CSkeleton::_SerializeBone(_stream, bone);
	}
}