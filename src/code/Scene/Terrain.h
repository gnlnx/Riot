/*********************************************************\
File:       Terrain.h
Purpose:    Defines terrain based on heightmap
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "Common.h"
#include "Scene\Object.h"

// CVertex
typedef struct _CVertex CVertex;

// CTerrain
class CTerrain: public CObject
{
public:
    // CTerrain constructor
    CTerrain( void );

    // CTerrain constructor
    CTerrain( const char* szFilename, const uint nWidth, const uint nHeight );

    // CTerrain destructor
    ~CTerrain( void );
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  SetHeightMap
    //  Set the file, width, and height to be used as the height map
    //-----------------------------------------------------------------------------
    void SetHeightMap( const char* szFilename, const uint nWidth, const uint nHeight );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    float GetHeight( float fX, float fZ );

    /***************************************\
    | mesh info methods                     |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    void* GetVertices( void );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    uint GetNumVertices( void );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    uint GetVertexStride( void );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    void* GetIndices( void );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    uint GetNumIndices( void );

    //-----------------------------------------------------------------------------
    //  GetHeight
    //  Get the height of a specific position
    //-----------------------------------------------------------------------------
    uint GetIndexSize( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/

    byte* m_ppHeightMap;
    uint m_nWidth;
    uint m_nHeight;  // TODO: rename to something more appropriate
    
    CVertex* m_pMeshVertices;
    uint m_nVertexStride;
    uint m_nNumVertices;
    uint* m_pMeshIndices;
    uint m_nIndexFormat;
    uint m_nNumIndices;

};

#endif // #ifndef _TERRAIN_H_
