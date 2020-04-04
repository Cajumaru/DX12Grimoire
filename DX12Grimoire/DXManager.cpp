#include "DXManager.h"

RESULTMESSAGE SDXManager::Initialize(const HWND& hwnd, UINT window_width, UINT window_height)
{
	HRESULT result;

	// フィーチャーレベル一覧
	const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// DXGIの作成 p68-69
#ifdef _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG
	ReturnResMes(result, "Use Latest Graphics Card");


	// アダプターの検索 p69
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	_adapter = nullptr;

	for (IDXGIAdapter* adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);

		wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			_adapter = adpt;
			break;
		}
	}

	if (_adapter == nullptr)
	{
		return RESULTMESSAGE(E_FAIL, "Use NVIDIA Graphics Card");
	}


	// デバイスの作成
	D3D_FEATURE_LEVEL featureLevel;
	for (D3D_FEATURE_LEVEL l : levels) {
		result = D3D12CreateDevice(_adapter, l, IID_PPV_ARGS(&_dev));
		if (!FAILED(result)) {
			featureLevel = l;
			break;
		}
	}
	ReturnResMes(result, "Device Failed");


	// コマンドアロケーターを作成
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	ReturnResMes(result, "Command Allocator Failed");


	// コマンドリストを作成
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	ReturnResMes(result, "Command List Failed");


	// コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// タイムアウト無し
	cmdQueueDesc.NodeMask = 0;										// アダプタが1つなのでマスクは指定しない
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// プライオリティ設定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// リストとタイプを合わせる


	// コマンドキューを作成
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
	ReturnResMes(result, "CommandQueue Failed");


	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	// スワップチェインの作成
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain);
	ReturnResMes(result, "Swapchain Failed");


	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; // 表用と裏用
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


	// ディスクリプタヒープの作成
	rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	ReturnResMes(result, "RT View Descriptor Failed");


	// スワップチェインから設定を読み込む
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);
	ReturnResMes(result, "Swapchain Aborted");


	// レンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;


	// スワップチェインの各バッファをリソースとして取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	for (unsigned int idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		_backBuffers.push_back(nullptr);
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		//DebugResultWithWindow(result, "Use Latest Graphics Card", "Buffer Assign Failed");

		// 取得したバッファにビューを関連付け
		_dev->CreateRenderTargetView(_backBuffers[idx], &rtvDesc, rtvH);
		rtvH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}


	// フェンスオブジェクトの作成
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	ReturnResMes(result, "Fence Failed");


	return RESULTMESSAGE(S_OK, "DOne");
}