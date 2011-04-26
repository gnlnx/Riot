/*********************************************************\
File:       UI.cpp
Purpose:    User interface...renders to a separate target
\*********************************************************/
#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include "UI.h"
#include "Riot.h"
#include "Gfx\D3DGraphics.h"

// UI vertex definition
typedef struct _UIVertex
{
    XMVECTOR vPos;
    XMVECTOR vColor;
} UIVertex;

// UI vertex element description
D3D11_INPUT_ELEMENT_DESC pVertexLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT nNumVertexLayoutElements = ARRAYSIZE( pVertexLayout );

// static members
ID3D11VertexShader* UI::m_pVertexShader = NULL;
ID3D11PixelShader* UI::m_pPixelShader = NULL;
ID3D11InputLayout* UI::m_pInputLayout = NULL;
wchar_t* UI::m_szShaderFile = L"Assets/Shaders/UI.hlsl";

// constructor
UI::UI( void )
    : m_nX( 0 )
    , m_nY( 0 )
{
    HRESULT hr = S_OK;

    // TODO: needs to be generic
    CD3DGraphics* pGraphics = ( CD3DGraphics* )Riot::GetGraphics();

    m_pDevice = pGraphics->GetDevice();
    m_pContext = pGraphics->GetDeviceContext();

    //////////////////////////////////////////
    // Load vertex shader
    ID3DBlob*   pShaderBlob = NULL;
    ID3DBlob*   pErrorBlob = NULL;
    uint nCompileFlags = 0;
#ifdef DEBUG
    nCompileFlags = D3DCOMPILE_DEBUG;
#endif
    hr = D3DX11CompileFromFile(  m_szShaderFile, // Filename
                                 NULL,           // Array of macro definitions
                                 NULL,           // #include interface
                                 "VS",           // Function name
                                 "vs_4_0",       // Shader profile
                                 nCompileFlags,  // Compile flags
                                 0,              // Not used for shaders, only effects
                                 NULL,           // Thread pump
                                 &pShaderBlob,   // Compiled code
                                 &pErrorBlob,    // Errors
                                 NULL );         // HRESULT

    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, (wchar_t*)pErrorBlob->GetBufferPointer(), L"Error", 0 );
        SAFE_RELEASE( pErrorBlob );
    }
    SAFE_RELEASE( pErrorBlob );

    // Now create the shader
    hr = m_pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVertexShader );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create UI vertex shader", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    }

    //////////////////////////////////////////
    // Load the vertex layout
    hr = m_pDevice->CreateInputLayout( pVertexLayout, 
                                       nNumVertexLayoutElements, 
                                       pShaderBlob->GetBufferPointer(), 
                                       pShaderBlob->GetBufferSize(), 
                                       &m_pInputLayout );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create the UI input layout", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    } 
    SAFE_RELEASE( pShaderBlob );

    //////////////////////////////////////////
    // Load pixel shader
#ifdef DEBUG
    nCompileFlags = D3DCOMPILE_DEBUG;
#endif
    hr = D3DX11CompileFromFile(  m_szShaderFile,    // Filename
                                 NULL,          // Array of macro definitions
                                 NULL,          // #include interface
                                 "PS",          // Function name
                                 "ps_4_0",      // Shader profile
                                 nCompileFlags, // Compile flags
                                 0,             // Not used for shaders, only effects
                                 NULL,          // Thread pump
                                 &pShaderBlob,  // Compiled code
                                 &pErrorBlob,   // Errors
                                 NULL );        // HRESULT

    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, (wchar_t*)pErrorBlob->GetBufferPointer(), L"Error", 0 );
        SAFE_RELEASE( pErrorBlob );
    }
    SAFE_RELEASE( pErrorBlob );

    // Now create the shader
    hr = m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPixelShader );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create UI pixel shader", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    }
}

UI::~UI( void )
{
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( m_pInputLayout );
}

void UI::PutText( uint nLeft, uint nTop, char* szText )
{
    m_nX = nLeft;
    m_nY = nTop;

    // Create vertex buffer
    D3D11_BUFFER_DESC       bufferDesc  = { 0 };
    D3D11_SUBRESOURCE_DATA  initData    = { 0 };
    HRESULT                 hr          = S_OK;

    UIVertex pVertices[] = 
    {
        { XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f ), XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f ), XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f ) },
        { XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f ), XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f ) },
    };
    unsigned int nNumVertices = 3;

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof( UIVertex ) * nNumVertices;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    initData.pSysMem = pVertices;

    ID3D11Buffer* pVertexBuffer = NULL;
    hr = m_pDevice->CreateBuffer( &bufferDesc, &initData, &pVertexBuffer );

    // Set the shaders
    m_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
    m_pContext->PSSetShader( m_pPixelShader, NULL, 0 );
    
    // Draw text
    unsigned int nStrides[] = { sizeof( UIVertex ) };
    unsigned int nOffsets[] = { 0 };
    m_pContext->IASetInputLayout( m_pInputLayout );
    m_pContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, nStrides, nOffsets );
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_pContext->Draw( 3, 0 );

    SAFE_RELEASE( pVertexBuffer );
}
