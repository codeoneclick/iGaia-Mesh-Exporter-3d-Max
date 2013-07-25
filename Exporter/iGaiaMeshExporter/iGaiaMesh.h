#ifndef __IGAIAMESH_H__
#define __IGAIAMESH_H__

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"

static std::string FixBoneName(const std::string& boneName)
{
	std::string fixedName = boneName;
	if (!fixedName.empty() )
	{

		std::replace( fixedName.begin(), fixedName.end(), ' ', '_' );
		std::transform( fixedName.begin(), fixedName.end(), fixedName.begin(), tolower );
	}
	return fixedName;
}

class CBone
{
private:

protected:

	IGameNode* m_globalNode;
	int	m_globalNodeId;
	std::string m_name;
	int	m_id;
	int	m_parentId;

	CBone* m_parent;
	CBone* m_next;	
	CBone* m_child;

public:

	CBone(void)
	{
		m_globalNode = NULL;
		m_globalNodeId = -1;
		m_id = -1;
		m_parentId = -1;
		m_parent = NULL;
		m_next = NULL;
		m_child = NULL;
	};

	CBone::~CBone()
	{

	};

	void Set_GlobalNode(IGameNode* _globalNode)
	{
		m_globalNode = _globalNode;
	}

	void Set_GlobalNodeId(int _id)
	{
		m_globalNodeId = _id;
	}

	void Set_Name(const std::string& _name)
	{
		m_name = FixBoneName(_name);
	}

	void Set_Id(int _id)
	{
		m_id = _id;
	}

	void Set_ParentId(int _parentId)
	{
		m_parentId = _parentId;
	}


	IGameNode* Get_GlobalNode(void) const
	{
		return m_globalNode;
	}

	int GetGlobalNodeId(void) const
	{
		return m_globalNodeId;
	}

	const std::string& Get_Name(void) const
	{
		return m_name;
	}

	int Get_Id(void) const
	{
		return m_id;
	}

	int Get_ParentId(void) const
	{
		return m_parentId;
	}

	void LinkChildBone(CBone* _child)
	{
		CBone* oldchild = m_child;
		m_child = _child;
		m_child->m_next = oldchild;
		m_child->m_parent = this;

		m_child->Set_ParentId(this->Get_Id());
	}

	CBone* Get_Parent(void) const
	{
		return m_parent;
	}

	CBone* Get_Child(void) const
	{
		return m_child;
	}

	CBone* Get_Next(void) const
	{
		return m_next;
	}
};

class Skeleton
{
private:

protected:

	std::vector<CBone*> m_bones;
	CBone* m_root;

public:

	Skeleton() {}
	~Skeleton() {}

	int Build(IGameObject* object)
	{
		IGameSkin* skin = object->GetIGameSkin();
		if(!skin)
		{
			return FALSE;
		}
		if(IGameSkin::IGAME_SKIN != skin->GetSkinType())
		{
			return FALSE;
		}

		int type, bone_id;
		const int numSkinnedVerts = skin->GetNumOfSkinnedVerts();

		for (int i = 0; i < numSkinnedVerts; ++i )
		{
			type = skin->GetVertexType( i );

			if ( IGameSkin::IGAME_RIGID == type )
			{
				bone_id = skin->GetBoneID( i, 0 );
				this->AddMaxBone( skin->GetIGameBone( i, 0 ), bone_id );
			}
			else
			{
				for(int j = 0; j < skin->GetNumberOfBones( i ); ++j )
				{
					bone_id = skin->GetBoneID( i, j );
					this->AddMaxBone( skin->GetIGameBone( i, j ), bone_id );
				}
			}
		}

	this->LinkBones();
	return TRUE;
}

int GetNumBones( void ) const
{
	return (int)m_bones.size();
}

CBone* GetRawBone( int i ) const
{
	return m_bones[i];
}

CBone* GetRootBone( void ) const
{
	return m_root;
}

int GetBoneIDByMaxID( int maxBoneID ) const
{
	CBone* bone = this->FindBoneByMaxID( maxBoneID );
	if ( !bone )
		return -1;
	return bone->Get_Id();
}

void WriteToStream(FILE* pFile)
{
	int numBones = this->GetNumBones();
	fwrite(&numBones, sizeof(int), 1, pFile);
	this->WriteBone(pFile, this->GetRootBone());
}

void AddMaxBone( IGameNode* boneNode, int maxBoneID )
{
	if ( NULL != this->FindBoneByMaxID( maxBoneID ) )
		return;

	CBone* newBone = new CBone();
	newBone->Set_GlobalNode(boneNode);
	newBone->Set_GlobalNodeId(maxBoneID);
	newBone->Set_Name(boneNode->GetName());
	newBone->Set_Id(this->GetNumBones());
	m_bones.push_back(newBone);
}

CBone* FindBoneByMaxID(int maxBoneID) const
{
	for (std::vector<CBone*>::const_iterator it = m_bones.begin(); it != m_bones.end(); ++it )
	{
		if ( (*it)->GetGlobalNodeId() == maxBoneID )
			return (*it);
	}

	return NULL;
}

void Skeleton::LinkBones( void )
{
	for (std::vector<CBone*>::const_iterator it = m_bones.begin(); it != m_bones.end(); ++it )
		this->LinkBone( *it );
}

void Skeleton::LinkBone(CBone* bone )
{
	IGameNode* node = bone->Get_GlobalNode();
	IGameNode* parent = node->GetNodeParent();
	CBone* parentBone = NULL;

	if ( parent )
		parentBone = this->FindBoneByMaxID( parent->GetNodeID() );

	if ( !parentBone )
		m_root = bone;
	else
		parentBone->LinkChildBone( bone );
}

void Skeleton::WriteBone(FILE* pFile, CBone* bone )
{
	//ss << bone->GetName() << " " << bone->GetID() << " " << bone->GetParentID() << std::endl;
	int boneId = bone->Get_Id();
	int boneParentId = bone->Get_ParentId();
	fwrite(&boneId, sizeof(int), 1, pFile);
	fwrite(&boneParentId, sizeof(int), 1, pFile);

	bone = bone->Get_Child();
	while(bone)
	{
		this->WriteBone(pFile, bone);
		bone = bone->Get_Next();
	}
}

};


class VertexWeight
{
public:

	VertexWeight() : m_Weight(0), m_BoneID(-1) {}
	VertexWeight( float w, int b ) : m_Weight(w), m_BoneID(b) {}

	float	m_Weight;
	int		m_BoneID;
};


class iGaiaVertex
{
public:
	Point3 m_vPosition;
	Point3 m_vNormal;
	Point3 m_vTexCoord;
	Point3 m_vTangent;
	int m_id;
	std::vector<VertexWeight> m_Weights;

	bool operator==(const iGaiaVertex& _vertex)const
	{
		return (m_vPosition == _vertex.m_vPosition && m_vNormal == _vertex.m_vNormal && m_vTexCoord == _vertex.m_vTexCoord);
	}
};

class AnimFrame
{
public:

	AnimFrame( int numBones )
	{
		m_BonesTransform.resize( numBones );
	}

	void AddBoneTransform( const GMatrix& transform, int boneID )
	{
		m_BonesTransform[boneID] = transform;
	}

	const GMatrix& GetBoneTransform( int i ) const
	{
		return m_BonesTransform[i];
	}

	int GetNumTransform( void ) const
	{
		return (int)m_BonesTransform.size();
	}

private:

	typedef	std::vector<GMatrix>	bonesTransformVec;

	bonesTransformVec		m_BonesTransform;
};



class AnimTrack
{
public:

	int Build( IGameObject* object, Skeleton* skeleton )
	{
		int i, j, time;

		int animStart = (int)( GetCOREInterface()->GetAnimRange().Start() / GetTicksPerFrame() );
		int animEnd   = (int)( GetCOREInterface()->GetAnimRange().End() / GetTicksPerFrame() );

		const int numAnimFrames = ( animEnd - animStart ) + 1;
		const int numBones = skeleton->GetNumBones();

		IGameNode* node;
		CBone* bone, * parent;

		m_AnimFrames.reserve( numAnimFrames );

		for ( i = 0; i < numAnimFrames; ++i )
		{
			time = i * GetTicksPerFrame();
			AnimFrame frame( numBones );

			for ( j = 0; j < numBones; ++j )
			{
				bone = skeleton->GetRawBone( j );
				node = bone->Get_GlobalNode();
				GMatrix transform = node->GetWorldTM( time );

				if ( bone->Get_ParentId() != -1 )
				{
					parent = bone->Get_Parent();
					node = parent->Get_GlobalNode();
					GMatrix parentTM = node->GetWorldTM( time );
					if ( -1 == parentTM.Parity() )
					{
						parentTM = parentTM.Inverse();
						GMatrix m = parentTM;
						memset( &parentTM, 0, sizeof( parentTM ) );
						parentTM -= m;
					}
					else
						parentTM = parentTM.Inverse();

					transform *= parentTM;
				}

				frame.AddBoneTransform( transform, bone->Get_Id() );
			}

			m_AnimFrames.push_back( frame );
		}

		return TRUE;
	}

	void WriteToStream(FILE* pFile)
	{
		int i, j;
		int numFrames = this->GetNumFrames();
		fwrite(&numFrames, sizeof(int), 1, pFile);

		for ( i = 0; i < this->GetNumFrames(); ++i )
		{
			const AnimFrame& frame = this->GetAnimFrame( i );
			for ( j = 0; j < frame.GetNumTransform(); ++j )
			{
				const GMatrix& mat = frame.GetBoneTransform( j );
				Point3 offset = mat.Translation();
				Quat rotation = mat.Rotation();

				fwrite(&offset.x, sizeof(float), 1, pFile);
				fwrite(&offset.y, sizeof(float), 1, pFile);
				fwrite(&offset.z, sizeof(float), 1, pFile);

				fwrite(&rotation.x, sizeof(float), 1, pFile);
				fwrite(&rotation.y, sizeof(float), 1, pFile);
				fwrite(&rotation.z, sizeof(float), 1, pFile);
				fwrite(&rotation.w, sizeof(float), 1, pFile);
			}
		}
	}



	int GetNumFrames( void ) const
	{
		return (int)m_AnimFrames.size();
	}

	const AnimFrame& GetAnimFrame( int i ) const
	{
		return m_AnimFrames[i];
	}


protected:

	typedef std::vector<AnimFrame>	framesVec;

	framesVec		m_AnimFrames;
};

class iGaiaMesh
{
protected:
	std::string				m_Name;
	std::vector<int>		m_IndexList;
	GMatrix					m_Matrix;
	std::vector<iGaiaVertex> pVertexData;
	Skeleton	m_skeleton;
	AnimTrack m_sequence;

public:

	enum
	{
		RET_OK = 0,
		RET_ERROR,
		RET_NOT_MESH,
		RET_NO_TEXCOORDS
	};

	iGaiaMesh() {}

	int CaptureVertexWeights( IGameObject* object, Skeleton* skeleton )
	{
		IGameSkin* skin = object->GetIGameSkin();
		if ( !skin )
			return FALSE;
		if ( IGameSkin::IGAME_SKIN != skin->GetSkinType() )
			return FALSE;

		const int numVerts = pVertexData.size();
		int i, j, numW, vID;

		for ( i = 0; i < numVerts; ++i )
		{
			vID = pVertexData[i].m_id;
			numW = skin->GetNumberOfBones( vID );

			if ( IGameSkin::IGAME_RIGID == skin->GetVertexType( vID ) )
				numW = 1;

			for ( j = 0; j < numW; ++j )
			{
				float w = skin->GetWeight( vID, j );
				if ( numW == 1 )
					w = 1.0f;
				int boneId = skeleton->GetBoneIDByMaxID( skin->GetBoneID( vID, j ) );
				pVertexData[i].m_Weights.push_back(VertexWeight(w, boneId));
			}
		}

		return TRUE;
	}

	int Build( IGameObject* object, IGameNode* node, const char* _sName )
	{
		if ( !object || !node )
			return RET_ERROR;
		if ( object->GetIGameType() != IGameMesh::IGAME_MESH )
			return RET_NOT_MESH;

		m_Name			= node->GetName();
		IGameMesh* mesh	= static_cast<IGameMesh*>(object);
		
		const int iNumVertexes  = mesh->GetNumberOfVerts();
		const int iNumTriangles	= mesh->GetNumberOfFaces();
		Tab<int> pTextureMap	= mesh->GetActiveMapChannelNum();

		if (pTextureMap.Count() <= 0)
		{
			return RET_NO_TEXCOORDS;
		}

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
				tVertex.m_id = pTriangle->vert[ j ];
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

		int flag;
		if(m_skeleton.Build(object))
		{
			flag = 1;
			fwrite(&flag, sizeof(int), 1, pFile);
			m_skeleton.WriteToStream(pFile);

			if(m_sequence.Build(object, &m_skeleton))
			{
				m_sequence.WriteToStream(pFile);
				if(CaptureVertexWeights(object, &m_skeleton))
				{
					for(unsigned int i = 0; i < pVertexData.size(); i++)
					{
						int numWeights = pVertexData[i].m_Weights.size();
						fwrite(&numWeights, sizeof(int), 1, pFile);
						for (int j = 0; j < numWeights; ++j )
						{
							int boneId = pVertexData[i].m_Weights[j].m_BoneID;
							float weight =  pVertexData[i].m_Weights[j].m_Weight;
							fwrite(&boneId, sizeof(int), 1, pFile);
							fwrite(&weight, sizeof(float), 1, pFile);
						}
					}
				}
				else
				{
					assert(false);
				}
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			flag = 0;
			fwrite(&flag, sizeof(int), 1, pFile);
		}

		fclose(pFile);
		return RET_OK;
	}
};

#endif	

