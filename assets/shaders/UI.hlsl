//--------------------------------------------------------------------------------------
// File: Terrain.hlsl
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color :  COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color: COLOR;
};


//--------------------------------------------------------------------------------------
// Vertex Shader -- Screen space
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return float4( 1, 1, 0, 1 );
}
