/*********************************************************\
File:       Terrain.cpp
Purpose:    Defines terrain based on heightmap
\*********************************************************/

#include "Terrain.h"
#include <cstdio>
#include <xnamath.h>

// CVertex
typedef struct _CVertex
{
    // Position
    XMVECTOR vPos;

    // Color
    XMVECTOR vColor;
} CVertex;

// CTerrain

static const uint s_nDefaultWidth = 256;
static const uint s_nDefaultHeight = 256;

// CTerrain constructor
CTerrain::CTerrain( void )
    : m_ppHeightMap( NULL )
    , m_nWidth( 0 )
    , m_nHeight( 0 )
    , m_pMeshVertices( NULL )
    , m_nVertexStride( sizeof(CVertex) )
    , m_nNumVertices( 0 )
    , m_pMeshIndices( NULL )
    , m_nIndexFormat( 32 )
    , m_nNumIndices( 0 )
{
    SetHeightMap( "Assets/textures/terrain.raw", s_nDefaultWidth, s_nDefaultHeight );
}

// CTerrain constructor
CTerrain::CTerrain( const char* szFilename, const uint nWidth, const uint nHeight )
    : m_ppHeightMap( NULL )
    , m_nWidth( 0 )
    , m_nHeight( 0 )
    , m_pMeshVertices( NULL )
    , m_nVertexStride( sizeof(CVertex) )
    , m_nNumVertices( 0 )
    , m_pMeshIndices( NULL )
    , m_nIndexFormat( 32 )
    , m_nNumIndices( 0 )
{
    SetHeightMap( szFilename, nWidth, nHeight );
}

// CTerrain destructor
CTerrain::~CTerrain( void )
{
    SAFE_DELETE_ARRAY( m_ppHeightMap );
    SAFE_DELETE_ARRAY( m_pMeshVertices );
    SAFE_DELETE_ARRAY( m_pMeshIndices );
}
/***************************************\
| class methods                         |
\***************************************/

//-----------------------------------------------------------------------------
//  SetHeightMap
//  Set the file, width, and height to be used as the height map
//-----------------------------------------------------------------------------
void CTerrain::SetHeightMap( const char* szFilename, const uint nWidth, const uint nHeight )
{
    // clear current height map
    if( m_ppHeightMap != NULL )
    {
        SAFE_DELETE_ARRAY( m_ppHeightMap );
        SAFE_DELETE_ARRAY( m_pMeshVertices );
        SAFE_DELETE_ARRAY( m_pMeshIndices );
        m_nNumVertices = 0;
        m_nNumIndices = 0;
    }

    // load new height map
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    uint nSize = m_nWidth * m_nHeight;
    m_ppHeightMap = new byte[ nSize ];
    m_pMeshVertices = new CVertex[ nSize ];
    m_pMeshIndices = new uint[ (m_nWidth - 1) * (m_nHeight - 1) * 3 * 2 ];

    // Read in heightmap info
    FILE* pFile;
    pFile = fopen( szFilename, "rb" );
    uint nBytesCopied = fread( m_ppHeightMap, sizeof(byte), nSize, pFile );
    fclose( pFile );

    // Vertices
    for( uint j = 0; j < m_nHeight; ++j )
    {
        for( uint i = 0; i < m_nWidth; ++i )
        {
            uint index  = ( m_nHeight * j ) + i;
            float height = m_ppHeightMap[ index ] / 4.0f;

            m_pMeshVertices[ index ].vPos = XMVectorSet( (float)i, height, (float)j, 1.0f );
            m_pMeshVertices[ index ].vColor = XMVectorSet( height / 256.0f, 0.0f, 0.0f, 1.0f );

            m_nNumVertices++;
        }
    }

    // Indices
    uint index = 0;
    for( uint jj = 0; jj < (m_nHeight - 1); ++jj )
    {
        for( uint ii = 0; ii < (m_nWidth - 1); ++ii )
        {
            uint k = (m_nHeight * jj) + ii;

            // Triangle 1
            m_pMeshIndices[ index     ] = k;                  // top left
            m_pMeshIndices[ index + 1 ] = m_nWidth + k;       // bottom left
            m_pMeshIndices[ index + 2 ] = m_nWidth + (k + 1); // bottom right

            // Triangle 2
            m_pMeshIndices[ index + 3 ] = k;                  // top left
            m_pMeshIndices[ index + 4 ] = m_nWidth + (k + 1); // bottom right
            m_pMeshIndices[ index + 5 ] = k + 1;              // top right

            index += 6;
        }
    }

    m_nNumIndices = index;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
float CTerrain::GetHeight( float fX, float fZ )
{
    return 0.0f;
}

/***************************************\
| mesh info methods                     |
\***************************************/

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
void* CTerrain::GetVertices( void )
{
    return m_pMeshVertices;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
uint CTerrain::GetNumVertices( void )
{
    return m_nNumVertices;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
uint CTerrain::GetVertexStride( void )
{
    return m_nVertexStride;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
void* CTerrain::GetIndices( void )
{
    return m_pMeshIndices;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
uint CTerrain::GetNumIndices( void )
{
    return m_nNumIndices;
}

//-----------------------------------------------------------------------------
//  GetHeight
//  Get the height of a specific position
//-----------------------------------------------------------------------------
uint CTerrain::GetIndexSize( void )
{
    return m_nIndexFormat;
}
