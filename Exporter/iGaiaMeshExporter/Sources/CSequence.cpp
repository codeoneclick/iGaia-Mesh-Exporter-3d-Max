#include "CSequence.h"
#include "CMesh.h"
#include "CBone.h"
#include "CSkeleton.h"

CSequence::CSequence(std::vector<IGameObject*>& gameObjects, CSkeleton* skeleton, const std::vector<SVertex>& vertexData) :
	m_gameObjects(gameObjects),
	m_skeleton(skeleton),
	m_vertexData(vertexData)
{

}

CSequence::~CSequence(void)
{

}

bool CSequence::bind(void)
{
		i32 startTime = static_cast<i32>(GetCOREInterface()->GetAnimRange().Start() / GetTicksPerFrame());
		i32 endTime = static_cast<i32>(GetCOREInterface()->GetAnimRange().End() / GetTicksPerFrame());

		const i32 numFrames = (endTime - startTime) + 1;
		const i32 numBones = m_skeleton->getNumBones();

		IGameNode *gameNode = nullptr;
		CBone *currentBone = nullptr, *parentBone = nullptr;

		for (i32 i = 0; i < numFrames; ++i)
		{
			i32 time = i * GetTicksPerFrame();
			CFrame frame(numBones);

			for (i32 j = 0; j < numBones; ++j)
			{
				currentBone = m_skeleton->getBoneWithBoneId(j);
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
		return true;
}

void CSequence::serialize(std::ofstream& stream)
{
	i32 numBones = m_skeleton->getNumBones();
	i32 numFrames = m_frames.size();
	stream.write((char*)&numBones, sizeof(i32));
	stream.write((char*)&numFrames, sizeof(i32));
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

			stream.write((char*)&_position, sizeof(glm::vec3));
			stream.write((char*)&_rotation, sizeof(glm::quat));
			stream.write((char*)&_scale, sizeof(glm::vec3));
		}
	}
}