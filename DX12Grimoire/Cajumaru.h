#pragma once
#include "Core.h"
#include "SimpleWindow.h"
#include "DXManager.h"
#include "Object.h"
#include "ObjectManager.h"

IDXGIFactory6* _dxgiFactory = nullptr;
ID3D12Device* _dev = nullptr;
ID3D12CommandAllocator* _cmdAllocator = nullptr;
ID3D12GraphicsCommandList* _cmdList = nullptr;
ID3D12CommandQueue* _cmdQueue = nullptr;
IDXGISwapChain4* _swapchain = nullptr;
ID3D12Fence* _fence = nullptr;

ID3D12DescriptorHeap* rtvHeaps = nullptr;
vector<ID3D12Resource*> _backBuffers;

UINT64 _fenceVal = 0;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


