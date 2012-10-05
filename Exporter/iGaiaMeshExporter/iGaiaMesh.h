#ifndef __IGAIAMESH_H__
#define __IGAIAMESH_H__

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"

class iGaiaMesh
{
protected:
	std::string				m_Name;
	std::vector<int>		m_IndexList;
	GMatrix					m_Matrix;

class iGaiaVertex
{
public:
	Point3 m_vPosition;
	Point3 m_vNormal;
	Point3 m_vTexCoord;
	Point3 m_vTangent;

	bool operator==(const iGaiaVertex& _vertex)const
	{
		return (m_vPosition == _vertex.m_vPosition && m_vNormal == _vertex.m_vNormal && m_vTexCoord == _vertex.m_vTexCoord);
	}
};
public:

	enum
	{
		RET_OK = 0,
		RET_ERROR,
		RET_NOT_MESH,
		RET_NO_TEXCOORDS
	};

	iGaiaMesh() {}

	int Build( IGameObject* obj, IGameNode* node, const char* _sName )
	{
		if ( !obj || !node )
			return RET_ERROR;
		if ( obj->GetIGameType() != IGameMesh::IGAME_MESH )
			return RET_NOT_MESH;

		m_Name			= node->GetName();
		IGameMesh* mesh	= static_cast<IGameMesh*>(obj);
		
		const int iNumVertexes  = mesh->GetNumberOfVerts();
		const int iNumTriangles	= mesh->GetNumberOfFaces();
		Tab<int> pTextureMap	= mesh->GetActiveMapChannelNum();

		if (pTextureMap.Count() <= 0)
		{
			return RET_NO_TEXCOORDS;
		}

		std::vector<iGaiaVertex> pVertexData;
		pVertexData.clear();
		std::vector<unsigned short> pIndexData;
		pIndexData.resize(iNumTriangles * 3);

		for(unsigned int i = 0; i < iNumTriangles; i++)
		{
			FaceEx* pTriangle = mesh->GetFace(i);
			for(unsigned int j = 0; j < 3; j++)
			{
				iGaiaVertex tVertex;
				tVertex.m_vPosition = mesh->GetVertex(pTriangle->vert[j]);
				tVertex.m_vNormal   = mesh->GetNormal(pTriangle->norm[j]);
				tVertex.m_vTangent  = mesh->GetTangent(pTriangle->vert[j]);
				DWORD pTexCoordIndexes[3];
				if (mesh->GetMapFaceIndex(pTextureMap[0], i, pTexCoordIndexes))
				{
					tVertex.m_vTexCoord = mesh->GetMapVertex(pTextureMap[0], pTexCoordIndexes[j]);
				}
				else
				{
					tVertex.m_vTexCoord = mesh->GetMapVertex(pTextureMap[0], pTriangle->vert[j]);
				}
				tVertex.m_vTexCoord.y = 1.0f - tVertex.m_vTexCoord.y;

				std::vector<iGaiaVertex>::iterator pIterator;
				unsigned short iIndex = 0;
				for(pIterator = pVertexData.begin(); pIterator != pVertexData.end(); ++pIterator, ++iIndex)
				{
					if(*pIterator == tVertex)
					{
						pIndexData[3 * i + j] = iIndex;
						break;
					}
				}

				if(pIterator == pVertexData.end())
				{
					pIndexData[3 * i + j] = iIndex;
					pVertexData.push_back(tVertex);
				}
			}
		}

		m_Matrix = node->GetObjectTM(0);
		if( -1 == m_Matrix.Parity())
		{
			std::vector<unsigned short> pTempIndexData;
			pTempIndexData.resize(pIndexData.size());
			for (unsigned int i = 0; i < (int)m_IndexList.size() / 3; i++)
			{
				pTempIndexData[3 * i + 0] = pIndexData[3 * i + 2];
				pTempIndexData[3 * i + 1] = pIndexData[3 * i + 1];
				pTempIndexData[3 * i + 2] = pIndexData[3 * i + 0];
			}
			pIndexData.swap(pTempIndexData);
		}

		int iCheckNumVertexes = pVertexData.size();
		int iCheckNumIndexes = pIndexData.size();

		FILE* pFile = fopen(_sName, "wb");
		if(!pFile)
		{
			return RET_ERROR;
		}

		fwrite(&iCheckNumVertexes, sizeof(int), 1, pFile);
		fwrite(&iCheckNumIndexes, sizeof(int), 1, pFile);
		for(unsigned int i = 0; i < pVertexData.size(); i++)
		{
			fwrite(&pVertexData[i].m_vPosition.x, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vPosition.y, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vPosition.z, sizeof(float), 1, pFile);

			fwrite(&pVertexData[i].m_vNormal.x, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vNormal.y, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vNormal.z, sizeof(float), 1, pFile);

			fwrite(&pVertexData[i].m_vTangent.x, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vTangent.y, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vTangent.z, sizeof(float), 1, pFile);

			fwrite(&pVertexData[i].m_vTexCoord.x, sizeof(float), 1, pFile);
			fwrite(&pVertexData[i].m_vTexCoord.y, sizeof(float), 1, pFile);
		}

		for(unsigned int i = 0; i < pIndexData.size(); i++)
		{
			fwrite(&pIndexData[i], sizeof(unsigned short), 1, pFile);
		}

		fclose(pFile);
		return RET_OK;
	}
};

#endif	

