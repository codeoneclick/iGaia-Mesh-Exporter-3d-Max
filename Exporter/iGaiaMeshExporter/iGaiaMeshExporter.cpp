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

#include "iGaiaMeshExporter.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"
#include "iGaiaMesh.h"
#include <sstream>
#include <fstream>

#define iGaiaMeshExporter_CLASS_ID	Class_ID(0x9339c6e0, 0x99611b32)

class iGaiaMeshExporter : public SceneExport 
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
		iGaiaMeshExporter();
		virtual ~iGaiaMeshExporter();

	private:
		static HWND hParams;
};



class iGaiaMeshExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new iGaiaMeshExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return iGaiaMeshExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("iGaiaMeshExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static iGaiaMeshExporterClassDesc iGaiaMeshExporterDesc;
ClassDesc2* GetiGaiaMeshExporterDesc() { return &iGaiaMeshExporterDesc; }





INT_PTR CALLBACK iGaiaMeshExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static iGaiaMeshExporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (iGaiaMeshExporter *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- GWExport -------------------------------------------------------
iGaiaMeshExporter::iGaiaMeshExporter()
{
}

iGaiaMeshExporter::~iGaiaMeshExporter() 
{
}

int iGaiaMeshExporter::ExtCount()
{
	return 1;
}

const TCHAR *iGaiaMeshExporter::Ext(int n)
{
	if ( !n )
		return _T("mdl");
	else
		return _T("");
}

const TCHAR *iGaiaMeshExporter::LongDesc()
{
	return _T("Exporter for iGaia 3d Engine");
}
	
const TCHAR *iGaiaMeshExporter::ShortDesc() 
{
	return _T("Exporter for iGaia 3d Engine");
}

const TCHAR *iGaiaMeshExporter::AuthorName()
{
	return _T("sergey.sergeev.codeoneclick ");
}

const TCHAR *iGaiaMeshExporter::CopyrightMessage() 
{
	return _T("");
}

const TCHAR *iGaiaMeshExporter::OtherMessage1() 
{
	return _T("");
}

const TCHAR *iGaiaMeshExporter::OtherMessage2() 
{
	return _T("");
}

unsigned int iGaiaMeshExporter::Version()
{
	return 100;
}

void iGaiaMeshExporter::ShowAbout(HWND hWnd)
{
	// Optional
}

BOOL iGaiaMeshExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int	iGaiaMeshExporter::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	int exportResult = FALSE;
	std::stringstream ss;
	std::ofstream os;

	IGameScene * pIgame = GetIGameInterface();

	IGameConversionManager* cm = GetConversionManager();
	cm->SetCoordSystem( IGameConversionManager::IGAME_OGL );

	pIgame->InitialiseIGame(true);
	pIgame->SetStaticFrame(0);

	Tab<IGameNode*> meshes = pIgame->GetIGameNodeByType( IGameObject::IGAME_MESH );
	if ( meshes.Count() == 0 )
		return FALSE;

	IGameNode* node = meshes[0];
	IGameObject* obj = node->GetIGameObject();
	obj->InitializeData();

	iGaiaMesh	expMesh;

	if ( expMesh.Build(obj, node, name) != iGaiaMesh::RET_OK )
		goto export_ends_here;
	exportResult = TRUE;

export_ends_here:

	node->ReleaseIGameObject();
	pIgame->ReleaseIGame();

	return exportResult;
}

