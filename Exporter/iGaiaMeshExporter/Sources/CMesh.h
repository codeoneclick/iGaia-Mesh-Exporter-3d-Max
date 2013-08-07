#ifndef CMesh_h
#define CMesh_h

#include "HCommon.h"

struct SBoneWeight
{
	i32 m_boneId;
	f32 m_weight;
};

struct SVertex
{
	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_texcoord;
	glm::vec3 m_tangent;
	i32 m_id;
	std::vector<SBoneWeight> m_weights;

	bool operator==(const SVertex& _vertex)const
	{
		return (m_position == _vertex.m_position && m_normal == _vertex.m_normal && m_texcoord == _vertex.m_texcoord);
	};
};

class CSkeleton;
class CSequence;


class CMesh
{
private:

	CSkeleton* m_skeleton;
	CSequence* m_sequence;

protected:

	std::vector<i32> m_indexData;
	std::vector<SVertex> m_vertexData;

	IGameObject* m_gameObject;
	IGameNode* m_gameNode;

	bool _BindMesh(void);

public:

	CMesh(IGameObject* _gameObject, IGameNode* _gameNode);
	~CMesh(void);

	bool Bind(void);
	void Serialize(std::ofstream& _stream);
};
#endif