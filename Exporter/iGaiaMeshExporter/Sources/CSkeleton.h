#ifndef CSkeleton_h
#define CSkeleton_h

#include "HCommon.h"

class CBone;
class CSkeleton
{
private:

protected:

	std::vector<IGameObject*> m_gameObjects;
	std::vector<CBone*> m_bones;
	std::vector<CBone*> m_roots;

	void linkBone(IGameNode* gameNode, i32 gameNodeId);
	void linkBone(CBone* bone);
	CBone* getBoneWithGameNodeId(i32 gameNodeId);
	void serializeBone(std::ofstream& stream, CBone* bone);

public:

	CSkeleton(std::vector<IGameObject*>& gameObjects);
	~CSkeleton(void);

	inline i32 getNumBones(void)
	{
		return m_bones.size();
	};

	inline CBone* getBoneWithBoneId(i32 boneId)
	{
		return m_bones.at(boneId);
	};

	i32 getBoneId(i32 gameNodeId);
	i32 getBoneId(IGameNode *gameNode);

	bool bind(void);
	void serialize(std::ofstream& stream);
};

#endif