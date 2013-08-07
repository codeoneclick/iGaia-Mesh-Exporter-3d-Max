#include "CMesh.h"
#include "CSequence.h"
#include "CSkeleton.h"

CMesh::CMesh(IGameObject* _gameObject, IGameNode* _gameNode) :
	m_gameObject(_gameObject),
	m_gameNode(_gameNode),
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

bool CMesh::Bind(void)
{
	bool result = CMesh::_BindMesh();
	if(result)
	{
		m_skeleton = new CSkeleton(m_gameObject, m_gameNode);
		result = m_skeleton->Bind();
		if(result)
		{
			m_sequence = new CSequence(m_gameObject, m_skeleton, m_vertexData);
			result = m_sequence->Bind();
		}
	}
	return result;
}

bool CMesh::_BindMesh(void)
{
	if (m_gameObject == nullptr || m_gameNode == nullptr)
	{
		return false;
	}
	if (m_gameObject->GetIGameType() != IGameMesh::IGAME_MESH)
	{
		return false;
	}
	IGameMesh* mesh	= static_cast<IGameMesh*>(m_gameObject);
		
	const i32 numVertexes = mesh->GetNumberOfVerts();
	const i32 numTriangles = mesh->GetNumberOfFaces();
	Tab<int> textureMap	= mesh->GetActiveMapChannelNum();

	if (textureMap.Count() <= 0)
	{
		return false;
	}

	m_vertexData.clear();
	m_indexData.clear();

	m_indexData.resize(numTriangles * 3);

	SVertex vertex;
	for(i32 i = 0; i < numTriangles; ++i)
	{
		FaceEx* triangle = mesh->GetFace(i);
		for(i32 j = 0; j < 3; ++j)
		{
			Point3 position = mesh->GetVertex(triangle->vert[j]);
			vertex.m_position = glm::vec3(position.x, position.y, position.z);
			Point3 normal = mesh->GetNormal(triangle->norm[j]);
			vertex.m_normal = glm::vec3(normal.x, normal.y, normal.z);
			Point3 tangent = mesh->GetTangent(triangle->vert[j]);
			vertex.m_tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
			vertex.m_id = triangle->vert[j];

			DWORD texcoordIndexes[3];
			Point3 texcoord;
			if (mesh->GetMapFaceIndex(textureMap[0], i, texcoordIndexes))
			{
				texcoord = mesh->GetMapVertex(textureMap[0], texcoordIndexes[j]);
			}
			else
			{
				texcoord = mesh->GetMapVertex(textureMap[0], triangle->vert[j]);
			}
			vertex.m_texcoord = glm::vec2(texcoord.x, 1.0f - texcoord.y);

			ui32 index = 0;
			auto iterator = m_vertexData.begin();
			for(; iterator != m_vertexData.end(); ++iterator, ++index)
			{
				if((*iterator) == vertex)
				{
					m_indexData[3 * i + j] = index;
					break;
				}
			}

			if(iterator == m_vertexData.end())
			{
				m_indexData[3 * i + j] = index;
				m_vertexData.push_back(vertex);
			}
		}
	}

	GMatrix matrix = m_gameNode->GetObjectTM(0);
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
	}
	return true;
}

void CMesh::Serialize(std::ofstream& _stream)
{
	i32 numVertexes = m_vertexData.size();
	i32 numIndexes = m_indexData.size();
	_stream.write((char*)&numVertexes, sizeof(i32));
	_stream.write((char*)&numIndexes, sizeof(i32));

	for(i32 i = 0; i < m_vertexData.size(); ++i)
	{
		_stream.write((char*)&m_vertexData[i].m_position, sizeof(glm::vec3));
		_stream.write((char*)&m_vertexData[i].m_normal, sizeof(glm::vec3));
		_stream.write((char*)&m_vertexData[i].m_tangent, sizeof(glm::vec3));
		_stream.write((char*)&m_vertexData[i].m_texcoord, sizeof(glm::vec2));
	}

	for(i32 i = 0; i < m_indexData.size(); ++i)
	{
		_stream.write((char*)&m_indexData[i], sizeof(ui16));
	}
	if(m_skeleton != nullptr && m_sequence != nullptr)
	{
		i32 animation = 1;
		_stream.write((char*)&animation, sizeof(i32));
		m_skeleton->Serialize(_stream);
		m_sequence->Serialize(_stream);
	}
	else
	{
		i32 animation = 0;
		_stream.write((char*)&animation, sizeof(i32));
	}
}