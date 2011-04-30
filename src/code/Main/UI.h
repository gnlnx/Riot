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

class UI
{
public:
    UI( void );
    ~UI( void );

    void PutText( uint nLeft, uint nTop, char* szText );

private:
    float m_fScreenX;
    float m_fScreenY;

    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;
    static ID3D11VertexShader* m_pVertexShader;
    static ID3D11PixelShader* m_pPixelShader;
    static ID3D11InputLayout* m_pInputLayout;
    static ID3D11SamplerState* m_pFontSampler;
    static ID3D11Texture2D* m_pFontTexture;
    static ID3D11ShaderResourceView* m_pFontSRV;
    static ID3D11BlendState* m_pFontBlend;
    static wchar_t* m_szShaderFile;
};

#endif // _UI_H_