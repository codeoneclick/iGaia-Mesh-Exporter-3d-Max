#ifndef CSkeleton_h
#define CSkeleton_h

#include "HCommon.h"

class CBone;
class CSkeleton
{
private:

protected:

	IGameObject* m_gameObject;
	IGameNode* m_gameNode;

	std::vector<CBone*> m_bones;
	std::vector<CBone*> m_roots;

	void _LinkBone(IGameNode* _gameNode, i32 _gameNodeId);
	void _LinkBone(CBone* _bone);
	CBone* _Get_Bone(i32 _gameNodeId);
	void _SerializeBone(std::ofstream& _stream, CBone* _bone);

public:

	CSkeleton(IGameObject* _gameObject, IGameNode* _gameNode);
	~CSkeleton(void);

	inline i32 Get_NumBones(void)
	{
		return m_bones.size();
	};

	inline CBone* Get_Bone(i32 _boneId)
	{
		return m_bones[_boneId];
	};

	i32 Get_BoneId(i32 _gameNodeId);

	bool Bind(void);
	void Serialize(std::ofstream& _stream);
};

#endif