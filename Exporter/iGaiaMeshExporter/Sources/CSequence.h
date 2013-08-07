#ifndef CSequence_h
#define CSequence_h

#include "HCommon.h"


class CFrame
{
private:

protected:

	std::vector<GMatrix> m_transformations;

public:

	CFrame(i32 _numBones)
	{
		m_transformations.resize(_numBones);
	};

	inline void LinkTransformation(const GMatrix& _matrix, i32 _boneId)
	{
		m_transformations[_boneId] = _matrix;
	};

	inline const GMatrix& Get_Transformation(i32 _boneId)
	{
		return m_transformations[_boneId];
	};

	inline i32 Get_NumTransformations(void)
	{
		return m_transformations.size();
	};
};

class CSkeleton;
struct SVertex;

class CSequence
{
private:

protected:

	IGameObject* m_gameObject;
	CSkeleton* m_skeleton;
	std::vector<SVertex> m_vertexData;
	std::vector<CFrame> m_frames;

	bool _BindVertexWeights(void);

public:

	CSequence(IGameObject* _gameObject, CSkeleton* _skeleton, const std::vector<SVertex>& _vertexData);
	~CSequence(void);

	bool Bind(void);
	void Serialize(std::ofstream& _stream);
};

#endif