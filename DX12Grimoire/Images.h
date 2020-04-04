#pragma once
#include "CoreDX.h"

typedef struct TextureData
{
	const DirectX::Image* Image;
	DirectX::TexMetadata Metadata;
} TextureData;

namespace
{
	TextureData TextureNatsu;
}