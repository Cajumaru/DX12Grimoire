#include <BasicShaderHeader.hlsli>

float4 BasicPS(PSIn input) : SV_TARGET
{
	//return float4(input.uv, 1.f, 1.f);
	float2 uv = input.uv;
	uv.x *= 800.0;
	uv.y *= 600.0;

	float2 imageSize = float2(640.0, 1136.0);
	float width = imageSize.x / imageSize.y * 600.0;

	float4 dest = (abs(400.0 - uv.x) <= width * 0.5) ? tex.Sample(smp, float2((uv.x - 400.0 + width * 0.5) / width, input.uv.y)) : float4(0.0, 0.0, 0.0, 1.0);

	return dest;
}