#pragma once
#include "Core.h"
#include "SimpleWindow.h"

class SDXManager
{
public:
	RESULTMESSAGE Initialize(const HWND& hwnd, UINT window_width, UINT window_height);


	IDXGIFactory6* GetDXGIFactory() const { return _dxgiFactory; }
	ID3D12Device* GetDevice() const { return _dev; }
	ID3D12CommandAllocator* GetCommandAllocator() const { return _cmdAllocator; }
	ID3D12GraphicsCommandList* GetCommandList() const { return _cmdList; }
	ID3D12CommandQueue* GetCommandQueue() const { return _cmdQueue; }
	IDXGISwapChain4* GetSwapchain() const { return _swapchain; }
	ID3D12Fence* GetFence() const { return _fence; }

	ID3D12DescriptorHeap* GetRTVDescriptorHeap() const { return rtvHeaps; }
	vector<ID3D12Resource*> GetBackBuffers() const { return _backBuffers; }

	UINT64 GetFenceValue() const { return _fenceVal; }

private:
	IDXGIFactory6* _dxgiFactory = nullptr;
	ID3D12Device* _dev = nullptr;
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	ID3D12CommandQueue* _cmdQueue = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;	
	ID3D12Fence* _fence = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	vector<ID3D12Resource*> _backBuffers;

	IDXGIAdapter* _adapter = nullptr;
	UINT64 _fenceVal = 0;
};
