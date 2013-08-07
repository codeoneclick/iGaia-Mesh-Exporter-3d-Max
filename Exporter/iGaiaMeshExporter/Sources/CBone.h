#ifndef CBone_h
#define CBone_h

#include "HCommon.h"

class CBone
{
private:

protected:

	IGameNode* m_gameNode;
	i32	m_gameNodeId;
	std::string m_name;
	i32	m_id;
	i32	m_parentId;

	CBone* m_parent;
	std::vector<CBone*> m_childs;

public:

	CBone(void);
	~CBone(void);

	inline void Set_GameNode(IGameNode* _gameNode)
	{
		m_gameNode = _gameNode;
	};

	inline IGameNode* Get_GameNode(void) const
	{
		return m_gameNode;
	};

	inline void Set_GameNodeId(i32 _gameNodeId)
	{
		m_gameNodeId = _gameNodeId;
	};

	inline i32 Get_GameNodeId(void) const
	{
		return m_gameNodeId;
	};

	inline void Set_Name(const std::string& _name)
	{
		m_name = _name;
	};

	inline const std::string& Get_Name(void) const
	{
		return m_name;
	};

	inline void Set_Id(i32 _id)
	{
		m_id = _id;
	};

	inline i32 Get_Id(void) const
	{
		return m_id;
	};

	inline void Set_ParentId(i32 _parentId)
	{
		m_parentId = _parentId;
	};

	i32 Get_ParentId(void) const
	{
		return m_parentId;
	};

	CBone* Get_Parent(void) const
	{
		return m_parent;
	};

	std::vector<CBone*> Get_Childs(void) const
	{
		return m_childs;
	};

	void LinkBone(CBone* _bone);

};

#endif