/*********************************************************\
File:       Riot.cpp
Purpose:    Definition of the main engine
\*********************************************************/
#include "Common.h"
#include "Riot.h"
#include "Timer.h"
#include <stdio.h> // For printf
#include "Window.h"
#include "Gfx\Graphics.h"
#include "Scene\SceneGraph.h"
#include "Scene\Object.h"
#include "Scene\Terrain.h"
#include "Gfx\View.h"
#include "Gfx\Material.h"
#include "Scene\ComponentManager.h"
#include "UI.h"

#if defined( OS_WINDOWS )
#include "PlatformDependent\Win32Window.h"
#include "Gfx\D3DGraphics.h"
//#include "OpenGLDevice.h"
#elif defined( OS_OSX )
#include "PlatformDependent\OSXWindow.h"
#include "Gfx\GLGraphics.h"
#elif defined( OS_LINUX )
#include "PlatformDependent\LinuxWindow.h"
#include "Gfx\GLGraphics.h"
#endif
#include "Memory.h"
#define new DEBUG_NEW

uint                Riot::m_nFrameCount     = 0;
float               Riot::m_fElapsedTime    = 0.0f;
float               Riot::m_fRunningTime    = 0.0f;
RiotInput*          Riot::m_pInput          = NULL;
CWindow*            Riot::m_pMainWindow     = NULL;
CGraphics*          Riot::m_pGraphics       = NULL;
CSceneGraph*        Riot::m_pSceneGraph     = NULL;
CComponentManager*  Riot::m_pComponentManager = NULL;
CView*              Riot::m_pMainView       = NULL;

bool                Riot::m_bRunning        = true;
    
//-----------------------------------------------------------------------------
//  Run
//  Starts the engine/game. All variables are set programatically
//-----------------------------------------------------------------------------
void Riot::Run( void )
{
    //-----------------------------------------------------------------------------
    // Initialization
    // TODO: Parse command line
    Initialize();

    Timer timer; // TODO: Should the timer be a class member?
    timer.Reset();
    float fFPSTime = 0.0f; // TODO: What's the best way to calculate FPS?
    float fFPS = 0.0f;
    //-----------------------------------------------------------------------------
    while( m_bRunning )
    {
        //---------------------- Start of frame --------------------
        // pMessageSystem->ProcessMessages();
        // pSceneGraph->StartFrame();
        // pRender->StartFrame();
        m_pInput->PollInput();
        if( m_pInput->IsKeyDown( VK_ESCAPE ) )
            m_bRunning = false;

        // Add a box everytime UP arrow is pressed
        if( m_pInput->WasKeyPressed( VK_UP ) )
        {
            CObject* pObject = new CObject();
            CMesh*   pMesh = m_pGraphics->CreateMesh( L"lol not loading a mesh!" );
            pObject->SetMesh( pMesh );
            CMaterial* pMaterial = m_pGraphics->CreateMaterial( L"Assets/Shaders/StandardVertexShader.hlsl", "PS", "ps_4_0" );
            pObject->SetMaterial( pMaterial );
            m_pSceneGraph->AddObject( pObject );
            pObject->AddComponent( eComponentPosition );
        }

        // Move camera
        float fCameraSpeed = 10.0f;
        float fCameraRotationSpeed = fCameraSpeed * 0.15f;
        if( m_pInput->IsKeyDown( 'W' ) ) // forward
        {
            if( m_pInput->IsKeyDown( VK_CONTROL ) )
            {
                m_pMainView->RotateX( -m_fElapsedTime * fCameraRotationSpeed );
            }
            else
            {
                m_pMainView->TranslateZ( m_fElapsedTime * fCameraSpeed );
            }
        }
        if( m_pInput->IsKeyDown( 'A' ) ) // left
        {
            if( m_pInput->IsKeyDown( VK_CONTROL ) )
            {
                m_pMainView->RotateY( -m_fElapsedTime * fCameraRotationSpeed );
            }
            else
            {
                m_pMainView->TranslateX( -m_fElapsedTime * fCameraSpeed );
            }
        }
        if( m_pInput->IsKeyDown( 'S' ) ) // back
        {
            if( m_pInput->IsKeyDown( VK_CONTROL ) )
            {
                m_pMainView->RotateX( m_fElapsedTime * fCameraRotationSpeed );
            }
            else
            {
                m_pMainView->TranslateZ( -m_fElapsedTime * fCameraSpeed );
            }
        }
        if( m_pInput->IsKeyDown( 'D' ) ) // right
        {
            if( m_pInput->IsKeyDown( VK_CONTROL ) )
            {
                m_pMainView->RotateY( m_fElapsedTime * fCameraRotationSpeed );
            }
            else
            {
                m_pMainView->TranslateX( m_fElapsedTime * fCameraSpeed );
            }
        }
        if( m_pInput->IsKeyDown( 'E' ) ) // up
        {
            m_pMainView->TranslateY( m_fElapsedTime * fCameraSpeed );
        }
        if( m_pInput->IsKeyDown( 'Q' ) ) // down
        {
            m_pMainView->TranslateY( -m_fElapsedTime * fCameraSpeed );
        }

        //-------------------------- Frame -------------------------

        //////////////////////////////////////////
        // Update
        m_pSceneGraph->UpdateObjects( m_fElapsedTime );


        //////////////////////////////////////////
        // Render
        m_pGraphics->PrepareRender();

        // draw scene
        uint nNumRenderObjects = 0;
        CObject** ppObjects = m_pSceneGraph->GetRenderObjects( &nNumRenderObjects );
        m_pGraphics->Render( ppObjects, nNumRenderObjects );

        // draw some text
        char szFPS[ 255 ];
        XMVECTOR vCamPos = m_pMainView->GetPosition();
        sprintf_s( szFPS, 255, "Camera: (%f, %f, %f)", XMVectorGetX(vCamPos), XMVectorGetY(vCamPos), XMVectorGetZ(vCamPos) );
        UI::AddString( 10, 30, szFPS );

        UI::Draw();

        m_pGraphics->Present();

        //----------------------- End of frame ---------------------
        // Perform system messaging
        m_pMainWindow->ProcessMessages();

        // Perform timing
        ++m_nFrameCount;
        m_fElapsedTime = (float)timer.GetTime();
        if( m_fElapsedTime > 0.5f )
        {   // Protection from huge lapses when debugging
            m_fElapsedTime = 1.0f/60.0f;
        }
        m_fRunningTime += m_fElapsedTime;
        fFPSTime += m_fElapsedTime;
        // Calculate FPS every 16 frames
        if( ( m_nFrameCount & 0xF ) == 0x0 )
        {
            fFPS = 16.0f / fFPSTime;
            fFPSTime = 0.0f;
        }
        sprintf_s( szFPS, 255, "fps: %f", fFPS );
        UI::AddString( 10, 10, szFPS );
    }
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // Cleanup
    //-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
//  Initialize
//  Initializes the engine. This is called from Run
//-----------------------------------------------------------------------------
void Riot::Initialize( void )
{
    //////////////////////////////////////////
    // Create window
    uint nWindowWidth = 1024,
         nWindowHeight = 768; // TODO: Read in from file

    // Create the new window object...
#if defined( OS_WINDOWS )
    m_pMainWindow = new CWin32Window();
    m_pGraphics = new CD3DGraphics();
#elif defined( OS_OSX )
    m_pMainWindow = new COSXWindow();
    m_pGraphics = new CGLGraphics();
#elif defined( OS_LINUX )
    m_pMainWindow = new CLinuxWindow();
    m_pGraphics = new CGLGraphics();
#endif
    // ...then create the actual window
    m_pMainWindow->CreateMainWindow( nWindowWidth, nWindowHeight );
    // ...and finally the graphics device
    m_pGraphics->Initialize( m_pMainWindow );

    //////////////////////////////////////////
    // Create the input system
    m_pInput = new RiotInput();

    //////////////////////////////////////////
    // Create the UI
    UI::Initialize();

    //////////////////////////////////////////
    //  Get the scene graph
    m_pSceneGraph = CSceneGraph::GetInstance();

    //////////////////////////////////////////
    // Create the main view
    m_pMainView = new CView();
    m_pSceneGraph->AddView( m_pMainView );

    //////////////////////////////////////////
    // Create the component manager
    m_pComponentManager = CComponentManager::GetInstance();

    //////////////////////////////////////////
    // Define scene objects
    LoadLevel();
}

//-----------------------------------------------------------------------------
//  LoadLevel
//  Defines the scene objects.  Called from Initialize
//-----------------------------------------------------------------------------
void Riot::LoadLevel( void )
{
    //// box
    //CObject* pBox = new CObject();
    //CMesh*   pMesh = m_pGraphics->CreateMesh( L"lol not loading a mesh!" );
    //pBox->SetMesh( pMesh );
    //CMaterial* pMaterial = m_pGraphics->CreateMaterial( L"Assets/Shaders/StandardVertexShader.hlsl", "PS", "ps_4_0" );
    //pBox->SetMaterial( pMaterial );
    //m_pSceneGraph->AddObject( pBox );
    //pBox->AddComponent( eComponentPosition );

    // TODO: Load terrain
    CTerrain* pTerrain = new CTerrain();
    //CMesh* pTerrainMesh = m_pGraphics->CreateMesh( L"lol not loading a mesh!" );
    CMesh* pTerrainMesh = m_pGraphics->CreateMesh( pTerrain->GetVertices(), 
                                                   pTerrain->GetVertexStride(),
                                                   pTerrain->GetNumVertices(),
                                                   pTerrain->GetIndices(),
                                                   pTerrain->GetIndexSize(),
                                                   pTerrain->GetNumIndices() );
    pTerrain->SetMesh( pTerrainMesh );
    CMaterial* pTerrainMaterial = m_pGraphics->CreateMaterial( L"Assets/Shaders/Terrain.hlsl", "PS", "ps_4_0" );
    pTerrain->SetMaterial( pTerrainMaterial );
    m_pSceneGraph->AddObject( pTerrain );
    pTerrain->AddComponent( eComponentPosition );
}

//-----------------------------------------------------------------------------
//  Shutdown
//  Shuts down and cleans up the engine
//-----------------------------------------------------------------------------
void Riot::Shutdown( void )
{    
    SAFE_RELEASE( m_pInput );
    SAFE_RELEASE( m_pGraphics );
    SAFE_RELEASE( m_pMainWindow );
    UI::Destroy();

    //////////////////////////////////////////
    // This is the last thing called
    DumpMemoryLeaks( );
}

//-----------------------------------------------------------------------------
//  GetGraphics
//  Returns the current graphics interface
//-----------------------------------------------------------------------------
CGraphics* Riot::GetGraphics( void )
{
    return m_pGraphics;
}
