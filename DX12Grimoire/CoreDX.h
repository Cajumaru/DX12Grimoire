#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

#pragma region Typedef_DirectXMath
typedef DirectX::XMFLOAT2 vec2;
typedef DirectX::XMFLOAT3 vec3;
typedef DirectX::XMFLOAT4 vec4;
#pragma endregion

typedef struct Vertex {
	vec3 Position;
	vec2 UV;
} Vertex;

typedef struct TexRGBA {
	unsigned char R, G, B, A;
} TexRGBA;

typedef unsigned short Index;


// Alignmentに切り上げした値を返す
// @param Size		元のサイズ
// @param Alignment	そろえるサイズ
// @param return	アライメントをそろえたサイズ
static size_t AlignmentedSize(size_t Size, size_t Alignment)
{
	return Size + Alignment - Size % Alignment;
}

namespace DX
{
	// D3D12_HEAP_PROPERTY
	// @param Type					refer D3D12_HEAP_TYPE
	// @param CPUPageProperty		refer D3D12_CPU_PAGE_PROPERTY
	// @param MemoryPoolPreference	refer D3D12_MEMORY_POOL
	// @param CreationNodeMask		UINT : 0 = no mask, other = some mask for multiple GPU
	// @param VisibleNodeMask		UINT : 

	const D3D12_HEAP_PROPERTIES HEAP_PROPERTIES_DEFAULT_BUFFER =
	{
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	};

	const D3D12_HEAP_PROPERTIES HEAP_PROPERTIES_UPLOAD_BUFFER =
	{
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	};

	const D3D12_HEAP_PROPERTIES HEAP_PROPERTIES_TEXTURE_BUFFER =
	{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	};
}