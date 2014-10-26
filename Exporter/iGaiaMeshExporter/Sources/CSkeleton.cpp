#include "CSkeleton.h"
#include "CBone.h"

CSkeleton::CSkeleton(std::vector<IGameObject*>& gameObjects) :
	m_gameObjects(gameObjects)
{

}

CSkeleton::~CSkeleton(void)
{

}

void CSkeleton::linkBone(IGameNode* gameNode, i32 gameNodeId)
{
	if (nullptr != CSkeleton::getBoneWithGameNodeId(gameNodeId))
	{
		return;
	}

	CBone* bone = new CBone();
	bone->Set_GameNode(gameNode);
	bone->Set_GameNodeId(gameNodeId);
	bone->Set_Name(gameNode->GetName());
	bone->Set_Id(m_bones.size());
	m_bones.push_back(bone);
}


void CSkeleton::linkBone(CBone* bone)
{
	IGameNode* gameNode = bone->Get_GameNode();
	IGameNode* gameNodeParent = gameNode->GetNodeParent();
	CBone* parentBone = nullptr;

	if (gameNodeParent != nullptr)
	{
		parentBone = CSkeleton::getBoneWithGameNodeId(gameNodeParent->GetNodeID());
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

CBone* CSkeleton::getBoneWithGameNodeId(i32 gameNodeId)
{
	std::vector<CBone*>::const_iterator iterator = m_bones.begin();
	for(; iterator != m_bones.end(); ++iterator)
	{
		if ((*iterator)->Get_GameNodeId() == gameNodeId)
		{
			return (*iterator);
		}
	}
	return nullptr;
}

i32  CSkeleton::getBoneId(i32 gameNodeId)
{
	CBone* bone = CSkeleton::getBoneWithGameNodeId(gameNodeId);
	if (bone == nullptr)
	{
		return -1;
	}
	return bone->Get_Id();
};

bool CSkeleton::bind(void)
{
	const size_t gameObjectsCount = m_gameObjects.size();
	for(size_t k = 0; k < gameObjectsCount; ++k)
	{
		IGameObject *gameObject = m_gameObjects.at(k);
		IGameSkin *skin	= gameObject->GetIGameSkin();
		if(skin == nullptr ||
		   IGameSkin::IGAME_SKIN != skin->GetSkinType())
		{
			continue;
		}

		i32 vertexType;
		const i32 numSkinnedVerts = skin->GetNumOfSkinnedVerts();

		for(i32 i = 0; i < numSkinnedVerts; ++i)
		{
			vertexType = skin->GetVertexType(i);
			if(IGameSkin::IGAME_RIGID == vertexType)
			{
				CSkeleton::linkBone(skin->GetIGameBone(i, 0), skin->GetBoneID(i, 0));
			}
			else
			{
				for(int j = 0; j < skin->GetNumberOfBones(i); ++j )
				{
					CSkeleton::linkBone(skin->GetIGameBone(i, j), skin->GetBoneID(i, j));
				}
			}
		}
	}

	std::vector<CBone*>::const_iterator iterator = m_bones.begin();
	for (; iterator != m_bones.end(); ++iterator)
	{
		CSkeleton::linkBone((*iterator));
	}
	return true;
}

void CSkeleton::serialize(std::ofstream& stream)
{
	i32 numBones = m_bones.size();
	stream.write((char*)&numBones, sizeof(i32));

	std::vector<CBone*>::iterator iterator = m_roots.begin();
	for(; iterator != m_roots.end(); ++iterator)
	{
		CBone* bone = (*iterator);
		CSkeleton::serializeBone(stream, bone);
	}
}

void CSkeleton::serializeBone(std::ofstream& stream, CBone* bone)
{
	i32 boneId = bone->Get_Id();
	i32 boneParentId = bone->Get_ParentId();
	stream.write((char*)&boneId, sizeof(i32));
	stream.write((char*)&boneParentId, sizeof(i32));

	std::vector<CBone*> childs = bone->Get_Childs();
	std::vector<CBone*>::iterator iterator = childs.begin();
	for(; iterator != childs.end(); ++iterator)
	{
		CBone* bone = (*iterator);
		CSkeleton::serializeBone(stream, bone);
	}
}