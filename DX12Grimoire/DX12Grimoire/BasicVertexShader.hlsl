#include <BasicShaderHeader.hlsli>

VSOut BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD) 
{
	VSOut output;
	output.svpos = pos;
	output.uv = uv;
	return output;
}