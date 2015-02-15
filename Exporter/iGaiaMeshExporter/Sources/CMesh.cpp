#include "CMesh.h"
#include "CSequence.h"
#include "CSkeleton.h"

CMesh::CMesh(std::vector<IGameNode*>& gameNodes, std::vector<IGameObject*>& gameObjects) :
	m_gameNodes(gameNodes),
	m_gameObjects(gameObjects),
	m_skeleton(nullptr),
	m_sequence(nullptr)
{
	
}

CMesh::~CMesh(void)
{
	delete m_skeleton;
	m_skeleton = nullptr;
	delete m_sequence;
	m_sequence = nullptr;
}

bool CMesh::bind(void)
{
	m_skeleton = new CSkeleton(m_gameObjects);
	bool result = m_skeleton->bind();

	if(result)
	{
		result = CMesh::bindMesh();
		if(result)
		{
			m_sequence = new CSequence(m_gameObjects, m_skeleton, m_vertexData);
			result = m_sequence->bind();
		}
	}
	return result;
}

bool CMesh::bindMesh(void)
{
	if(m_gameNodes.size() == 0 || 
		m_gameObjects.size() == 0 ||
		m_gameNodes.size() != m_gameObjects.size())
	{
		return false;
	}

	i32 totalNumVertexes = 0;
	i32 totalNumTriangles = 0;

	const size_t gameObjectsCount = m_gameObjects.size();
	for(size_t k = 0; k < gameObjectsCount; ++k)
	{
		IGameNode *gameNode = m_gameNodes.at(k);
		IGameObject *gameObject = m_gameObjects.at(k);
		if(gameObject->GetIGameType() != IGameMesh::IGAME_MESH)
		{
			continue;
		}
		IGameMesh *gameMesh	= static_cast<IGameMesh*>(gameObject);
		totalNumVertexes += gameMesh->GetNumberOfVerts();
		totalNumTriangles += gameMesh->GetNumberOfFaces();
	}

	m_vertexData.clear();
	m_indexData.clear();

	m_vertexData.resize(0);
	m_indexData.resize(totalNumTriangles * 3);
	i32 indicesOffset = 0;

	for(size_t k = 0; k < gameObjectsCount; ++k)
	{
		IGameObject *gameObject = m_gameObjects.at(k);
		IGameNode *gameNode = m_gameNodes.at(k);
		std::string gameNodeName = gameNode->GetName();
		std::cout<<"name: "<<gameNodeName<<std::endl;
		IGameNode *parentGameNode = gameNode->GetNodeParent();
		IGameObject *parentGameObject = parentGameNode != nullptr ? parentGameNode->GetIGameObject() : nullptr;
		std::string parentGameNodeName = parentGameNode != nullptr ? parentGameNode->GetName() : "";
		std::cout<<"parent name: "<<parentGameNodeName<<std::endl;

		if(gameObject->GetIGameType() != IGameMesh::IGAME_MESH)
		{
			continue;
		}
		IGameMesh *gameMesh	= static_cast<IGameMesh*>(gameObject);
		IGameSkin *gameSkin	= gameObject->GetIGameSkin();

		Tab<int> textureMap	= gameMesh->GetActiveMapChannelNum();
		if (textureMap.Count() <= 0)
		{
			return false;
		}

		i32 numVertexes = gameMesh->GetNumberOfVerts();
		i32 numTriangles = gameMesh->GetNumberOfFaces();
		
		for(i32 i = 0; i < numTriangles; ++i)
		{
			FaceEx *triangle = gameMesh->GetFace(i);
			for(i32 j = 0; j < 3; ++j)
			{
				SVertex vertex;
				i32 indexId = triangle->vert[j];
				Point3 position = gameMesh->GetVertex(indexId);
				vertex.m_position = glm::vec3(position.x, position.y, position.z);
				Point3 normal = gameMesh->GetNormal(triangle->norm[j]);
				vertex.m_normal = glm::vec3(normal.x, normal.y, normal.z);
				Point3 tangent = gameMesh->GetTangent(indexId);
				vertex.m_tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
				vertex.m_indexId = indexId;

				DWORD texcoordIndexes[3];
				Point3 texcoord;
				if (gameMesh->GetMapFaceIndex(textureMap[0], i, texcoordIndexes))
				{
					texcoord = gameMesh->GetMapVertex(textureMap[0], texcoordIndexes[j]);
				}
				else
				{
					texcoord = gameMesh->GetMapVertex(textureMap[0], indexId);
				}
				vertex.m_texcoord = glm::vec2(texcoord.x, 1.0 - texcoord.y);

				auto iterator = m_vertexData.begin();
				for(ui32 index = 0; iterator != m_vertexData.end(); ++iterator, ++index)
				{
					if((*iterator) == vertex)
					{
						m_indexData[3 * i + j + indicesOffset] = index;
						break;
					}
				}

				if(iterator == m_vertexData.end())
				{
					m_indexData[3 * i + j + indicesOffset] = m_vertexData.size();

					if(gameSkin != nullptr && 
						IGameSkin::IGAME_SKIN == gameSkin->GetSkinType())
					{
						i32 numWeights = gameSkin->GetNumberOfBones(vertex.m_indexId);

						if (IGameSkin::IGAME_RIGID == gameSkin->GetVertexType(vertex.m_indexId))
						{
							numWeights = 1;
						}

						for (i32 z = 0; z < numWeights; ++z)
						{
							f32 weight = gameSkin->GetWeight(vertex.m_indexId, z);
							if (numWeights == 1)
							{
								weight = 1.0f;
							}
							i32 boneId = m_skeleton->getBoneId(gameSkin->GetBoneID(vertex.m_indexId, z));
							SBoneWeight boneWeight;
							boneWeight.m_weight = weight;
							boneWeight.m_boneId = boneId;
							vertex.m_weights.push_back(boneWeight);
						}
					} else if(parentGameObject != nullptr && 
						parentGameObject->GetIGameType() == IGameMesh::IGAME_BONE)
					{
						i32 boneId =  m_skeleton->getBoneId(parentGameNode);
						SBoneWeight boneWeight;
						boneWeight.m_weight = 1.0;
						boneWeight.m_boneId = boneId;
						vertex.m_weights.push_back(boneWeight);
						std::cout<<"parent is bone"<<std::endl;
					}
					if(vertex.m_weights.size() > 4)
					{
						return false;
					}
					m_vertexData.push_back(vertex);
				}
			}
		}
		indicesOffset += numTriangles * 3;
	}

	/*GMatrix matrix = m_gameNode->GetObjectTM(0);
	if(-1 == matrix.Parity())
	{
		std::vector<i32> swapIndexData;
		swapIndexData.resize(m_indexData.size());
		for (ui32 i = 0; i < m_indexData.size() / 3; ++i)
		{
			swapIndexData[3 * i + 0] = m_indexData[3 * i + 2];
			swapIndexData[3 * i + 1] = m_indexData[3 * i + 1];
			swapIndexData[3 * i + 2] = m_indexData[3 * i + 0];
		}
		m_indexData.swap(swapIndexData);
	}*/
	return true;
}

void CMesh::serialize(const std::string& filename)
{
	std::ofstream stream;
	stream.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	if(!stream.is_open())
	{
		return;
	}

#if defined (__MESH__)

	i32 numVertexes = m_vertexData.size();
	i32 numIndexes = m_indexData.size();
	stream.write((char*)&numVertexes, sizeof(i32));
	stream.write((char*)&numIndexes, sizeof(i32));

	for(i32 i = 0; i < m_vertexData.size(); ++i)
	{
		stream.write((char*)&m_vertexData[i].m_position, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_normal, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_tangent, sizeof(glm::vec3));
		stream.write((char*)&m_vertexData[i].m_texcoord, sizeof(glm::vec2));

		i32 numWeights = m_vertexData[i].m_weights.size();
		stream.write((char*)&numWeights, sizeof(i32));
		for (i32 j = 0; j < numWeights; ++j)
		{
			i32 boneId = m_vertexData[i].m_weights[j].m_boneId;
			f32 weight = m_vertexData[i].m_weights[j].m_weight;
			stream.write((char*)&boneId, sizeof(i32));
			stream.write((char*)&weight, sizeof(f32));
		}
	}

	for(i32 i = 0; i < m_indexData.size(); ++i)
	{
		stream.write((char*)&m_indexData[i], sizeof(ui16));
	}

	m_skeleton->serialize(stream);
	stream.close();

#elif defined(__ANIMATION__)

	m_sequence->serialize(stream);
	stream.close();

#endif
}