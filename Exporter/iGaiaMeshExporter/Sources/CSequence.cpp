#include "CSequence.h"
#include "CMesh.h"
#include "CBone.h"
#include "CSkeleton.h"

CSequence::CSequence(IGameObject* _gameObject, CSkeleton* _skeleton, const std::vector<SVertex>& _vertexData) :
	m_gameObject(_gameObject),
	m_skeleton(_skeleton),
	m_vertexData(_vertexData)
{

}

CSequence::~CSequence(void)
{

}

bool CSequence::_BindVertexWeights(void)
	{
		IGameSkin* skin = m_gameObject->GetIGameSkin();
		if (skin == nullptr)
		{
			return false;
		}
		if (IGameSkin::IGAME_SKIN != skin->GetSkinType())
		{
			return false;
		}
			
		const i32 numVertexes = m_vertexData.size();
		i32 numWeights;

		for (i32 i = 0; i < numVertexes; ++i)
		{
			numWeights = skin->GetNumberOfBones(m_vertexData[i].m_id);

			if ( IGameSkin::IGAME_RIGID == skin->GetVertexType(m_vertexData[i].m_id))
			{
				numWeights = 1;
			}

			for (i32 j = 0; j < numWeights; ++j)
			{
				f32 weight = skin->GetWeight(m_vertexData[i].m_id, j);
				if (numWeights == 1)
				{
					weight = 1.0f;
				}
				i32 boneId = m_skeleton->Get_BoneId(skin->GetBoneID(m_vertexData[i].m_id, j));
				SBoneWeight boneWeight;
				boneWeight.m_weight = weight;
				boneWeight.m_boneId = boneId;
				m_vertexData[i].m_weights.push_back(boneWeight);
			}
		}

		return TRUE;
	}

bool CSequence::Bind(void)
{
		i32 startTime = static_cast<i32>(GetCOREInterface()->GetAnimRange().Start() / GetTicksPerFrame());
		i32 endTime = static_cast<i32>(GetCOREInterface()->GetAnimRange().End() / GetTicksPerFrame());

		const i32 numFrames = (endTime - startTime) + 1;
		const i32 numBones = m_skeleton->Get_NumBones();

		IGameNode* gameNode;
		CBone* currentBone, *parentBone;
		m_frames.reserve(numFrames);

		for (i32 i = 0; i < numFrames; ++i)
		{
			i32 time = i * GetTicksPerFrame();
			CFrame frame(numBones);

			for (i32 j = 0; j < numBones; ++j)
			{
				currentBone = m_skeleton->Get_Bone(j);
				gameNode = currentBone->Get_GameNode();

				IGameObject* gameObject = gameNode->GetIGameObject();

				IGameMesh* mesh	= static_cast<IGameMesh*>(gameObject);
		
				const i32 numVertexes = mesh->GetNumberOfVerts();
				const i32 numTriangles = mesh->GetNumberOfFaces();

				GMatrix matrix = gameNode->GetWorldTM(time);
				frame.LinkTransformation(matrix, currentBone->Get_Id());
			}

			m_frames.push_back(frame);
		}
		return CSequence::_BindVertexWeights();
}

void CSequence::Serialize(std::ofstream& _stream)
{
	i32 numFrames = m_frames.size();
	_stream.write((char*)&numFrames, sizeof(i32));
	for(i32 i = 0; i < m_frames.size(); ++i)
	{
		CFrame frame = m_frames[i];
		for(i32 j = 0; j < frame.Get_NumTransformations(); ++j)
		{
			GMatrix matrix = frame.Get_Transformation(j);
			Point3 position;
			Point3 scale;
			Quat rotation;
			Matrix3 decomposedMatrix = matrix.ExtractMatrix3();
			DecomposeMatrix(decomposedMatrix, position, rotation, scale);

			glm::vec3 _position = glm::vec3(position.x, position.y, position.z);
			glm::quat _rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
			glm::vec3 _scale = glm::vec3(scale.x, scale.y, scale.z);

			_stream.write((char*)&_position, sizeof(glm::vec3));
			_stream.write((char*)&_rotation, sizeof(glm::quat));
			_stream.write((char*)&_scale, sizeof(glm::vec3));
		}
	}

	for(i32 i = 0; i < m_vertexData.size(); ++i)
	{
		i32 numWeights = m_vertexData[i].m_weights.size();
		_stream.write((char*)&numWeights, sizeof(i32));
		for (int j = 0; j < numWeights; ++j )
		{
			i32 boneId = m_vertexData[i].m_weights[j].m_boneId;
			f32 weight = m_vertexData[i].m_weights[j].m_weight;
			_stream.write((char*)&boneId, sizeof(i32));
			_stream.write((char*)&weight, sizeof(f32));
		}
	}
}