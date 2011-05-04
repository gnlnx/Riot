/*********************************************************\
File:           UI.h
Purpose:        User interface elements...renders to a
                separate target
Author:         Omar A Rodriguez
Created:        4/24/2011
\*********************************************************/
#ifndef _UI_H_
#define _UI_H_
#include "Types.h"

class CGraphics;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11BlendState;
struct ID3D11Buffer;

//////////////////////////////////////////
// UI item definition
typedef struct _UIString
{
    uint nLeft;
    uint nTop;
    char szText[255];
} UIString;

class UI
{
//---------------------------------------------------------------------------------
//  Methods
public:
    //-----------------------------------------------------------------------------
    //  Initialize
    //  Define shaders, input layout, and font texture
    //-----------------------------------------------------------------------------
    static void Initialize( void );
    //-----------------------------------------------------------------------------
    //  AddText
    //  Add some text to the list of strings
    //-----------------------------------------------------------------------------
    static void AddString( uint nLeft, uint nTop, const char* szText );
    //-----------------------------------------------------------------------------
    //  DrawString
    //  Draw szText at (nLeft, nTop)
    //-----------------------------------------------------------------------------
    static void DrawString( uint nLeft, uint nTop, const char* szText );
    //-----------------------------------------------------------------------------
    //  Draw()
    //  Draw all the strings
    //-----------------------------------------------------------------------------
    static void Draw( void );
    //-----------------------------------------------------------------------------
    //  Destroy
    //  Release the memory allocated on Init
    //-----------------------------------------------------------------------------
    static void Destroy( void );

//---------------------------------------------------------------------------------
//  Members
private:
    static float m_fScreenX;
    static float m_fScreenY;

    static ID3D11Device* m_pDevice;
    static ID3D11DeviceContext* m_pContext;
    static ID3D11VertexShader* m_pVertexShader;
    static ID3D11PixelShader* m_pPixelShader;
    static ID3D11InputLayout* m_pInputLayout;
    static ID3D11SamplerState* m_pFontSampler;
    static ID3D11Texture2D* m_pFontTexture;
    static ID3D11ShaderResourceView* m_pFontSRV;
    static ID3D11BlendState* m_pFontBlend;
    static ID3D11Buffer* m_pVertexBuffer;
    static wchar_t* m_szShaderFile;

    static UIString* m_pUIStrings;
    static uint m_nNumStrings;
};

#endif // _UI_H_
