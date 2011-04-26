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
struct D3D11_INPUT_ELEMENT_DESC;

class UI
{
public:
    UI( void );
    ~UI( void );

    void PutText( uint nLeft, uint nTop, char* szText );

private:
    uint m_nX;
    uint m_nY;

    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;
    static ID3D11VertexShader* m_pVertexShader;
    static ID3D11PixelShader* m_pPixelShader;
    static ID3D11InputLayout* m_pInputLayout;
    static wchar_t* m_szShaderFile;
};

#endif // _UI_H_
