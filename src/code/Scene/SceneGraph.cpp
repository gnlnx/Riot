/*********************************************************\
File:           SceneGraph.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/23/2011 11:14:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "SceneGraph.h"
#include "memory.h"
#include "Object.h"
#include "Riot.h"
#include "Gfx\View.h"
#include "Gfx\Graphics.h"
#include "ComponentManager.h"
#include <memory> // for memcpy
#include "Main\UI.h"
#define new DEBUG_NEW

// CSceneGraph constructor
CSceneGraph::CSceneGraph()
    : m_ppAllSceneObjects( NULL )
    , m_ppRenderObjects( NULL )
    , m_nNumTotalObjects( 0 )
    , m_nNumRenderObjects( 0 )
    , m_nNumViews( 0 )
    , m_pActiveView( NULL )
{
    m_ppAllSceneObjects = new CObject*[MAX_OBJECTS];
    memset( m_ppAllSceneObjects, 0, sizeof( CObject* ) * MAX_OBJECTS );
    m_ppRenderObjects = new CObject*[MAX_OBJECTS];
    memset( m_ppRenderObjects, 0, sizeof( CObject* ) * MAX_OBJECTS );
}

CSceneGraph::~CSceneGraph()
{
    // clean up objects
    for( uint i = 0; i < m_nNumTotalObjects; ++i )
    {
        SAFE_DELETE( m_ppAllSceneObjects[i] );
    }
    SAFE_DELETE_ARRAY( m_ppAllSceneObjects );
    SAFE_DELETE_ARRAY( m_ppRenderObjects );
}

//-----------------------------------------------------------------------------
//  GetInstance
//  Gets/creates the one instance of the class
//-----------------------------------------------------------------------------
CSceneGraph* CSceneGraph::GetInstance( void )
{
    static CSceneGraph pInstance;
    return &pInstance;
}


//-----------------------------------------------------------------------------
//  AddObject
//  Adds an object to the scene
//-----------------------------------------------------------------------------
void CSceneGraph::AddObject( CObject* pObject )
{
    m_ppAllSceneObjects[ m_nNumTotalObjects++ ] = pObject;
}


//-----------------------------------------------------------------------------
//  AddView
//  Adds a view to the scene
//-----------------------------------------------------------------------------
void CSceneGraph::AddView( CView* pView )
{
    m_ppViews[ m_nNumViews++ ] = pView;
    AddObject( (CObject*)pView );

    m_pActiveView = pView; // TODO: Figure out where to do this
}


//-----------------------------------------------------------------------------
//  UpdateObjects
//  Updates all the objects
//-----------------------------------------------------------------------------
void CSceneGraph::UpdateObjects( float fDeltaTime )
{
    // Each object still has an Update for anything super specialized it might need?
    // Hmm.......
    Riot::GetGraphics()->SetCurrentView( m_pActiveView );
    for( uint i = 0; i < m_nNumTotalObjects; ++i )
    {
        m_ppAllSceneObjects[i]->Update( ((i%2) == 0) ? fDeltaTime : -fDeltaTime );
    }

    // Update the components
    CComponentManager::GetInstance()->ProcessComponents();

    // Update the current view
    m_pActiveView->Update( fDeltaTime );

    char szNumObj[ 255 ];
    sprintf_s( szNumObj, 255, "Total objects in scengraph: %d", m_nNumTotalObjects );
    UI::AddString( 10, 50, szNumObj );
}

//-----------------------------------------------------------------------------
//  GetRenderObjects
//  Returns all objects in view that need to be rendered
//-----------------------------------------------------------------------------
CObject** CSceneGraph::GetRenderObjects( uint* nCount )
{
    memcpy( m_ppRenderObjects, m_ppAllSceneObjects, sizeof( CObject* ) * m_nNumTotalObjects );
    *nCount = m_nNumRenderObjects = m_nNumTotalObjects;

    char szNumObj[ 255 ];
    sprintf_s( szNumObj, 255, "Total objects rendered: %d", m_nNumTotalObjects );
    UI::AddString( 10, 70, szNumObj );

    return m_ppRenderObjects;
}
