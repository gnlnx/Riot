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

//////////////////////////////////////////
// UI vertex definition
typedef struct _UIVertex
{
    XMVECTOR vPos;
    XMVECTOR vColor;
    XMVECTOR vTexcoord;  // xy = uv...zw = ??
} UIVertex;

//////////////////////////////////////////
// UI vertex element description
D3D11_INPUT_ELEMENT_DESC pVertexLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT nNumVertexLayoutElements = ARRAYSIZE( pVertexLayout );

//////////////////////////////////////////
// static members
float                      UI::m_fScreenX      = 0.0f;
float                      UI::m_fScreenY      = 0.0f;
ID3D11Device*              UI::m_pDevice       = NULL;
ID3D11DeviceContext*       UI::m_pContext      = NULL;
ID3D11VertexShader*        UI::m_pVertexShader = NULL;
ID3D11PixelShader*         UI::m_pPixelShader  = NULL;
ID3D11InputLayout*         UI::m_pInputLayout  = NULL;
ID3D11SamplerState*        UI::m_pFontSampler  = NULL;
ID3D11Texture2D*           UI::m_pFontTexture  = NULL;
ID3D11ShaderResourceView*  UI::m_pFontSRV      = NULL;
ID3D11BlendState*          UI::m_pFontBlend    = NULL;
ID3D11Buffer*              UI::m_pVertexBuffer = NULL;
wchar_t*                   UI::m_szShaderFile  = L"Assets/Shaders/UI.hlsl";
static const uint          gs_nMaxNumChars     = 255 * 6;

static const uint          gs_nMaxNumStrings   = 100;
UIString*                  UI::m_pUIStrings    = new UIString[ gs_nMaxNumStrings ];
uint                       UI::m_nNumStrings   = 0;

//-----------------------------------------------------------------------------
//  Initialize
//  Define shaders, input layout, and font texture
//-----------------------------------------------------------------------------
void UI::Initialize( void )
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
    //////////////////////////////////////////
    // Load the font texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    D3D11_TEXTURE2D_DESC desc;

    hr = D3DX11CreateTextureFromFile( m_pDevice, L"Assets/Textures/Font.dds", NULL, NULL, (ID3D11Resource**)(&m_pFontTexture), NULL );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Failed to load texture", L"Error", 0 );
    }

    m_pFontTexture->GetDesc( &desc );

    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    hr = m_pDevice->CreateShaderResourceView( m_pFontTexture, &srvDesc, &m_pFontSRV );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Failed to create the shader resource view", L"Error", 0 );
    }

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof( D3D11_SAMPLER_DESC ) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = m_pDevice->CreateSamplerState( &sampDesc, &m_pFontSampler );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Failed to create the texture sampler", L"Error", 0 );
    }

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory( &blendDesc, sizeof( D3D11_BLEND_DESC ) );
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    hr = m_pDevice->CreateBlendState( &blendDesc, &m_pFontBlend );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Failed to create the blend sampler", L"Error", 0 );
    }
    //////////////////////////////////////////
    // Init the vertex buffer
    // Create vertex buffer
    D3D11_BUFFER_DESC       bufferDesc      = { 0 };
    D3D11_SUBRESOURCE_DATA  initData        = { 0 };
    ID3D11Buffer*           pVertexBuffer   = NULL;
    UIVertex*               pVertices       = new UIVertex[ gs_nMaxNumChars ];
    
    for( uint i = 0; i < gs_nMaxNumChars; ++i )
    {
        pVertices[ i ].vPos = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
        pVertices[ i ].vColor = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
        pVertices[ i ].vTexcoord = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    }

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof( UIVertex ) * gs_nMaxNumChars;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    initData.pSysMem = pVertices;

    hr = m_pDevice->CreateBuffer( &bufferDesc, &initData, &m_pVertexBuffer );
    SAFE_DELETE_ARRAY( pVertices );
}

//-----------------------------------------------------------------------------
//  Destroy
//  Release the memory allocated on Init
//-----------------------------------------------------------------------------
void UI::Destroy( void )
{
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pPixelShader );
    SAFE_RELEASE( m_pInputLayout );
    SAFE_RELEASE( m_pFontSampler );
    SAFE_RELEASE( m_pFontTexture );
    SAFE_RELEASE( m_pFontSRV );
    SAFE_RELEASE( m_pVertexBuffer );
}

//-----------------------------------------------------------------------------
//  AddText
//  Add some text to the list of strings
//-----------------------------------------------------------------------------
void UI::AddString( uint nLeft, uint nTop, const char* szText )
{
    m_pUIStrings[ m_nNumStrings ].nLeft = nLeft;
    m_pUIStrings[ m_nNumStrings ].nTop = nTop;
    strcpy_s( m_pUIStrings[ m_nNumStrings ].szText, 255, szText );
    m_nNumStrings++;
}

//-----------------------------------------------------------------------------
//  Draw()
//  Draw all the strings
//-----------------------------------------------------------------------------
void UI::Draw( void )
{
    // draw all strings
    for( uint i = 0; i < m_nNumStrings; ++i )
    {
        uint nLeft = m_pUIStrings[ i ].nLeft;
        uint nTop = m_pUIStrings[ i ].nTop;
        const char* szText = m_pUIStrings[ i ].szText;
        DrawString( nLeft, nTop, szText );
    }

    m_nNumStrings = 0;
}

//-----------------------------------------------------------------------------
//  DrawString
//  Draw szText at (nLeft, nTop)
//-----------------------------------------------------------------------------
void UI::DrawString( uint nLeft, uint nTop, const char* szText )
{
    float fScaleFactor = 2.0f;
    float fCharWidth = (950.0f / 95.0f) / 950.0f;
    float fFontWidth = ( ( 950.0f / 95.0f ) / 1024.0f ) * fScaleFactor;
    float fFontHeight = (20.0f / 768.0f) * fScaleFactor;
    uint nNumChars = strlen( szText );

    // adjust screen coords to have (0, 0) at the top left
    // and increase X left -> right and Y top -> bottom
    m_fScreenX = 2.0f * ( nLeft / 1024.0f ) - 1.0f;  // [-1, 1]
    m_fScreenY = -2.0f * ( nTop / 768.0f ) + 1.0f - fFontHeight; // [1-font_height, -1-font_height]

    // Vertices info
    uint nNumVertices = nNumChars * 6;
    UIVertex* pVertices = new UIVertex[ nNumVertices ];
    uint j = 0;
    
    // Create quads for the string
    for( uint i = 0; i < nNumChars; ++i )
    {
        float fCurrentScreenX = m_fScreenX + ( i * fFontWidth );
        float fLeftX = fCurrentScreenX;
        float fRightX = fCurrentScreenX + (fCharWidth * fScaleFactor);
        float fTopY = m_fScreenY + fFontHeight;
        float fBottomY = m_fScreenY;

        char cCurrentChar = szText[i];
        uint nAsciiPos = cCurrentChar - 32;
        float fTexcoord_x0 = nAsciiPos * fCharWidth;      // left
        float fTexcoord_x1 = fTexcoord_x0 + fCharWidth;   // right
        float fTexcoord_y0 = 0.0f;                        // top
        float fTexcoord_y1 = 1.0f;                        // bottom

        j = i * 6;

        // Triangle 1
        // left bottom
        pVertices[ j + 0 ].vPos      = XMVectorSet( fLeftX,  fBottomY, 0.0f, 1.0f );
        pVertices[ j + 0 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 0 ].vTexcoord = XMVectorSet( fTexcoord_x0, fTexcoord_y1, 0.0f, 0.0f );
        // left top
        pVertices[ j + 1 ].vPos      = XMVectorSet( fLeftX,  fTopY, 0.0f, 1.0f );
        pVertices[ j + 1 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 1 ].vTexcoord = XMVectorSet( fTexcoord_x0, fTexcoord_y0, 0.0f, 0.0f );
        // right top
        pVertices[ j + 2 ].vPos      = XMVectorSet( fRightX, fTopY, 0.0f, 1.0f );
        pVertices[ j + 2 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 2 ].vTexcoord = XMVectorSet( fTexcoord_x1, fTexcoord_y0, 0.0f, 0.0f );
        // Triangle 2
        // left bottom
        pVertices[ j + 3 ].vPos      = XMVectorSet( fLeftX,  fBottomY, 0.0f, 1.0f );
        pVertices[ j + 3 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 3 ].vTexcoord = XMVectorSet( fTexcoord_x0, fTexcoord_y1, 0.0f, 0.0f );
        // right top
        pVertices[ j + 4 ].vPos      = XMVectorSet( fRightX, fTopY, 0.0f, 1.0f );
        pVertices[ j + 4 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 4 ].vTexcoord = XMVectorSet( fTexcoord_x1, fTexcoord_y0, 0.0f, 0.0f );
        // right bottom
        pVertices[ j + 5 ].vPos      = XMVectorSet( fRightX, fBottomY, 0.0f, 1.0f );
        pVertices[ j + 5 ].vColor    = XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f );
        pVertices[ j + 5 ].vTexcoord = XMVectorSet( fTexcoord_x1, fTexcoord_y1, 0.0f, 0.0f );
    }

    // Update the vertex buffer
    D3D11_MAPPED_SUBRESOURCE pMappedVB;
    m_pContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMappedVB );
    UIVertex* pMappedVertices = ( UIVertex* )pMappedVB.pData;
    memcpy_s( pMappedVertices, gs_nMaxNumChars * sizeof( UIVertex ), pVertices, nNumChars * 6 * sizeof( UIVertex ) );
    m_pContext->Unmap( m_pVertexBuffer, 0 );

    // Set the shaders
    m_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
    m_pContext->PSSetShader( m_pPixelShader, NULL, 0 );

    // Set shader stuff
    m_pContext->PSSetSamplers( 0, 1, &m_pFontSampler );
    m_pContext->PSSetShaderResources( 0, 1, &m_pFontSRV );
    m_pContext->OMSetBlendState( m_pFontBlend, 0, 0xFFFFFFFF );

    // Draw text
    unsigned int nStrides[] = { sizeof( UIVertex ) };
    unsigned int nOffsets[] = { 0 };
    m_pContext->IASetInputLayout( m_pInputLayout );
    m_pContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, nStrides, nOffsets );
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_pContext->Draw( nNumVertices, 0 );

    SAFE_DELETE_ARRAY( pVertices );
}
