//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "ie3Dexporter.h"
#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"
#include <sstream>
#include <fstream>
#include "Sources/CMesh.h"

#if defined(__MESH__)

#define ie3Dexporter_CLASS_ID Class_ID(0x9339c6e0, 0x99611b32)

#elif defined(__ANIMATION__)

#define ie3Dexporter_CLASS_ID Class_ID(0x9339c6e1, 0x99611b33)

#endif

class ie3Dexporter : public SceneExport 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		ie3Dexporter();
		virtual ~ie3Dexporter();

	private:
		static HWND hParams;
};



class ie3DexporterClassDescription : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new ie3Dexporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return ie3Dexporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }
#if defined (__MESH__)
	virtual const TCHAR* InternalName() 			{ return _T("ie3Dmesh"); }
#elif defined (__ANIMATION__)
	virtual const TCHAR* InternalName() 			{ return _T("ie3Danimation"); }
#endif
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static ie3DexporterClassDescription ie3DexporterDescription;
ClassDesc2* getIE3DexporterDescription() { return &ie3DexporterDescription; }

INT_PTR CALLBACK iGaiaMeshExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ie3Dexporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (ie3Dexporter*)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


ie3Dexporter::ie3Dexporter()
{
}

ie3Dexporter::~ie3Dexporter() 
{
}

int ie3Dexporter::ExtCount()
{
	return 1;
}

const TCHAR *ie3Dexporter::Ext(int n)
{
	if ( !n )
#if defined(__MESH__)
		return _T("ie3Dmesh");
#elif defined (__ANIMATION__)
		return _T("ie3Danimation");
#endif
	else
		return _T("");
}

const TCHAR *ie3Dexporter::LongDesc()
{
#if defined(__MESH__)
		return _T("ie3D engine mesh export");
#elif defined (__ANIMATION__)
		return _T("ie3D engine animation export");
#endif
}
	
const TCHAR *ie3Dexporter::ShortDesc() 
{
#if defined(__MESH__)
		return _T("ie3D engine mesh export");
#elif defined (__ANIMATION__)
		return _T("ie3D engine animation export");
#endif
}

const TCHAR *ie3Dexporter::AuthorName()
{
	return _T("sergey.sergeev.codeoneclick ");
}

const TCHAR *ie3Dexporter::CopyrightMessage() 
{
	return _T("");
}

const TCHAR *ie3Dexporter::OtherMessage1() 
{
	return _T("");
}

const TCHAR *ie3Dexporter::OtherMessage2() 
{
	return _T("");
}

unsigned int ie3Dexporter::Version()
{
	return 100;
}

void ie3Dexporter::ShowAbout(HWND hWnd)
{
	// Optional
}

BOOL ie3Dexporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int	ie3Dexporter::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options)
{
	int exportResult = FALSE;

	IGameScene * pIgame = GetIGameInterface();
	IGameConversionManager* cm = GetConversionManager();
	cm->SetCoordSystem( IGameConversionManager::IGAME_OGL);

	pIgame->InitialiseIGame(true);
	pIgame->SetStaticFrame(0);

	Tab<IGameNode*> gameMeshes = pIgame->GetIGameNodeByType(IGameObject::IGAME_MESH);
	i32 numGameMeshes = gameMeshes.Count();
	
	if (numGameMeshes == 0)
	{
		return false;
	}

	std::vector<IGameNode*> gameNodes;
	std::vector<IGameObject*> gameObjects;

	for(size_t i = 0;  i < numGameMeshes; ++i)
	{
		IGameNode *gameNode = gameMeshes[i];
		IGameObject *gameObject = gameNode->GetIGameObject();
		if(gameNode != nullptr &&
			gameObject != nullptr && 
			gameObject->GetIGameType() == IGameMesh::IGAME_MESH)
		{
			gameObject->InitializeData();
			gameNodes.push_back(gameNode);
			gameObjects.push_back(gameObject);
		}
	}

	CMesh mesh(gameNodes, gameObjects);
	if(!mesh.bind())
	{
		goto export_ends_here;
	}

	mesh.serialize(name);
	exportResult = TRUE;
export_ends_here:

	pIgame->ReleaseIGame();

	return exportResult;
}

