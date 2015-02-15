#ifndef CMesh_h
#define CMesh_h

#include "HCommon.h"

struct SBoneWeight
{
	i32 m_boneId;
	f32 m_weight;
};

inline bool isEqual(f32 value01, f32 value02)
{
	 return fabsf(value01 - value02) < FLT_EPSILON;
}

inline bool isEqual(const glm::vec2& value01, const glm::vec2& value02)
{
	return isEqual(value01.x, value02.x) &&
		isEqual(value01.y, value02.y);
}

inline bool isEqual(const glm::vec3& value01, const glm::vec3& value02)
{
	return isEqual(value01.x, value02.x) &&
		isEqual(value01.y, value02.y) &&
		isEqual(value01.z, value02.z);
}

struct SVertex
{
	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_texcoord;
	glm::vec3 m_tangent;
	i32 m_indexId;
	//i32 m_uniqueId;

	std::vector<SBoneWeight> m_weights;

	bool operator==(const SVertex& vertex)const
	{
		//return m_uniqueId == vertex.m_uniqueId;
		return isEqual(m_position, vertex.m_position) &&
			isEqual(m_texcoord, vertex.m_texcoord);
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

	std::vector<IGameNode*> m_gameNodes;
	std::vector<IGameObject*> m_gameObjects;

	bool bindMesh(void);

public:

	CMesh(std::vector<IGameNode*>& gameNodes, std::vector<IGameObject*>& gameObjects);
	~CMesh(void);

	bool bind(void);
	void serialize(const std::string& filename);
};
#endif