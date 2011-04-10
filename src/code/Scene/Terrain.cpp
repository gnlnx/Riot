/*********************************************************\
File:       Terrain.cpp
Purpose:    Defines terrain based on heightmap
\*********************************************************/

#include "Terrain.h"
#include <cstdio>

// CVertex
typedef struct _CVertex
{
    // Position
    float X;
    float Y;
    float Z;

    // Color
    float R;
    float G;
    float B;
    float A;
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

    // Vertices
    FILE* pFile;
    pFile = fopen( szFilename, "rb" );

    for( uint j = 0; j < m_nHeight; ++j )
    {
        for( uint i = 0; i < m_nWidth; ++i )
        {
            uint index  = ( m_nHeight * j ) + i;
            float height = fgetc( pFile ) / 4.0f;
            m_pMeshVertices[ index ].X = i;
            m_pMeshVertices[ index ].Y = height;
            m_pMeshVertices[ index ].Z = j;

            m_pMeshVertices[ index ].R = height / 256.0f;
            m_pMeshVertices[ index ].G = 0.0f;
            m_pMeshVertices[ index ].B = 0.0f;
            m_pMeshVertices[ index ].A = 1.0f;
            m_nNumVertices++;
        }
    }

    fclose( pFile );

    // Indices
    uint index = 0;
    for( uint jj = 0; jj < (m_nHeight - 1); ++jj )
    {
        for( uint ii = 0; ii < (m_nWidth - 1); ++ii )
        {
            uint k = (m_nHeight * jj) + ii;

            // Triangle 1
            m_pMeshIndices[ index     ] = k;
            m_pMeshIndices[ index + 1 ] = m_nWidth + k;
            m_pMeshIndices[ index + 2 ] = m_nWidth + (k + 1);

            // Triangle 2
            m_pMeshIndices[ index + 3 ] = k;
            m_pMeshIndices[ index + 4 ] = m_nWidth + (k + 1);
            m_pMeshIndices[ index + 5 ] = k + 1;

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
