#include "Cajumaru.h"

void EnableDebugLayer();


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
#pragma region WindowGenerate
	UPOINTER(SSimpleWindow, iSimpleWindow, hInstance, lpCmdLine, nCmdShow);;

	iSimpleWindow->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
#pragma endregion

#ifdef _DEBUG
	EnableDebugLayer();
#endif // _DEBUG

#pragma region DX12Init
	HRESULT result;

	// �t�B�[�`���[���x���ꗗ
	const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// DXGI�̍쐬 p68-69
#ifdef _DEBUG
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG

	DebugResultWithWindow(result, "Use Latest Graphics Card", "FactoryFailed");


	// �A�_�v�^�[�̌��� p69
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}

	IDXGIAdapter* TargetAdapter = nullptr;

	for (IDXGIAdapter* adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);

		wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			TargetAdapter = adpt;
			break;
		}
	}

	if (TargetAdapter == nullptr)
	{
		MessageBox(NULL, L"Use NVIDIA Graphics Card", L"Error", MB_OK);
		return 0;
	}


	// �f�o�C�X�̍쐬
	D3D_FEATURE_LEVEL featureLevel;
	for (D3D_FEATURE_LEVEL l : levels) {
		result = D3D12CreateDevice(TargetAdapter, l, IID_PPV_ARGS(&_dev));
		if (!FAILED(result)) {
			featureLevel = l;
			break;
		}
	}
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Device Failed");


	// �R�}���h�A���P�[�^�[���쐬
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Allocator Failed");


	// �R�}���h���X�g���쐬
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "CommandList Failed");


	// �R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;				// �^�C���A�E�g����
	cmdQueueDesc.NodeMask = 0;										// �A�_�v�^��1�Ȃ̂Ń}�X�N�͎w�肵�Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// �v���C�I���e�B�ݒ�Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;				// ���X�g�ƃ^�C�v�����킹��


	// �R�}���h�L���[���쐬
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "CommandQueue Failed");


	// �X���b�v�`�F�C���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = WINDOW_WIDTH;
	swapchainDesc.Height = WINDOW_HEIGHT;
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


	// �X���b�v�`�F�C���̍쐬
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		iSimpleWindow->GetWindowHandle(),
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain);

	DebugResultWithWindow(result, "Use Latest Graphics Card", "Swapchain Failed");


	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; // �\�p�Ɨ��p
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


	// �f�B�X�N���v�^�q�[�v�̍쐬
	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Descriptor Heap Failed");


	// �X���b�v�`�F�C������ݒ��ǂݍ���
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Failed to Get Descriptor from Swapchain");


	// �����_�[�^�[�Q�b�g�r���[�ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;


	// �X���b�v�`�F�C���̊e�o�b�t�@�����\�[�X�Ƃ��Ď擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (unsigned int idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		DebugResultWithWindow(result, "Use Latest Graphics Card", "Buffer Assign Failed");

		// �擾�����o�b�t�@�Ƀr���[���֘A�t��
		_dev->CreateRenderTargetView(_backBuffers[idx], &rtvDesc, rtvH);
		rtvH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}


	// �t�F���X�I�u�W�F�N�g�̍쐬
	ID3D12Fence* _fence;
	UINT64 _fenceVal = 0;

	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Fence Failed");
#pragma endregion

	iSimpleWindow->Show_Window();

#pragma region SetUpDrawObjectData
	// ���_���
	Vertex vertices[] = {
		{vec3(-1.f, -1.f, 0.f),	vec2(0.f, 1.f)},	// ����
		{vec3(-1.f, 1.f, 0.f), vec2(0.f, 0.f)},		// ����
		{vec3(1.f, -1.f, 0.f), vec2(1.f, 1.f)},		// �E��
		{vec3(1.f, 1.f, 0.f), vec2(1.f, 0.f) },		// �E��
	};


	// �C���f�b�N�X�f�[�^
	unsigned short indices[] = {
	0, 1, 2,
	2, 1, 3
	};


	// �V�F�[�_�[���\�[�X(�e�N�X�`��)
	/*
	vector<TexRGBA> testTextureData(256 * 256);
	for (TexRGBA& TexData : testTextureData)
	{
		TexData.R = rand() % 256;
		TexData.G = rand() % 256;
		TexData.B = rand() % 256;
		TexData.A = 255;
	}
	*/

	DirectX::ScratchImage LoadedImageCache = {};
	DirectX::TexMetadata LoadedImageMetaData = {};

	result = DirectX::LoadFromWICFile(L"Natsu1.jpg", DirectX::WIC_FLAGS_NONE, &LoadedImageMetaData, LoadedImageCache);
	const DirectX::Image* LoadedImage = LoadedImageCache.GetImage(0, 0, 0);


	// �q�[�v�̈�̐ݒ�
	D3D12_HEAP_PROPERTIES HeapProp = {};
	HeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProp.CreationNodeMask = 0;
	HeapProp.VisibleNodeMask = 0;

	// ���ԃo�b�t�@(�A�b�v���[�h�q�[�v)�p
	D3D12_HEAP_PROPERTIES UploadHeapProp = {};
	UploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	UploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	UploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	UploadHeapProp.CreationNodeMask = 0;
	UploadHeapProp.VisibleNodeMask = 0;

	// �e�N�X�`���̃A�b�v���[�h�p
	D3D12_HEAP_PROPERTIES TexHeapProp = {};
	/*
	// WriteToSubresource�p
	TexHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	TexHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	TexHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	TexHeapProp.CreationNodeMask = 0;
	TexHeapProp.VisibleNodeMask = 0;
	*/
	// CopyTextureRegion�p
	TexHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT; // GPU��ɃL���b�V���������
	TexHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	TexHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	TexHeapProp.CreationNodeMask = 0;
	TexHeapProp.VisibleNodeMask = 0;


	// ���\�[�X�ݒ�\����
	D3D12_RESOURCE_DESC ResDesc = {};
	ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResDesc.Width = sizeof(vertices);
	ResDesc.Height = 1;
	ResDesc.DepthOrArraySize = 1;
	ResDesc.MipLevels = 1;
	ResDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResDesc.SampleDesc.Count = 1;
	ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �A�b�v���[�h�p
	D3D12_RESOURCE_DESC UploadResDesc = {};
	UploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	UploadResDesc.Width = LoadedImage->slicePitch;
	UploadResDesc.Height = 1;
	UploadResDesc.Format = DXGI_FORMAT_UNKNOWN;
	UploadResDesc.DepthOrArraySize = 1;
	UploadResDesc.MipLevels = 1;
	UploadResDesc.SampleDesc.Count = 1;
	UploadResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	UploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �e�N�X�`���p�̐ݒ�
	D3D12_RESOURCE_DESC TexResDesc = {};
	/*
	// WriteToSubresource�p
	TexResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(LoadedImageMetaData.dimension);
	TexResDesc.Width = (UINT)LoadedImageMetaData.width;
	TexResDesc.Height = (UINT)LoadedImageMetaData.height;
	TexResDesc.Format = LoadedImageMetaData.format;
	TexResDesc.DepthOrArraySize = (UINT)LoadedImageMetaData.arraySize;
	TexResDesc.SampleDesc.Count = 1;
	TexResDesc.SampleDesc.Quality = 0;
	TexResDesc.MipLevels = (UINT)LoadedImageMetaData.mipLevels;
	TexResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	*/
	// CopyTextureRegion�p
	TexResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(LoadedImageMetaData.dimension);
	TexResDesc.Width = (UINT)LoadedImageMetaData.width;
	TexResDesc.Height = (UINT)LoadedImageMetaData.height;
	TexResDesc.Format = LoadedImageMetaData.format;
	TexResDesc.DepthOrArraySize = (UINT)LoadedImageMetaData.arraySize;
	TexResDesc.SampleDesc.Count = 1;
	TexResDesc.SampleDesc.Quality = 0;
	TexResDesc.MipLevels = (UINT)LoadedImageMetaData.mipLevels;
	TexResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	// ���\�[�X�̈�̊m��

	// ���_�o�b�t�@
	ID3D12Resource* vertBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&HeapProp,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// �C���f�b�N�X�o�b�t�@
	ResDesc.Width = sizeof(indices);

	ID3D12Resource* indBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&HeapProp,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indBuff));

	// �A�b�v���[�h�o�b�t�@
	ID3D12Resource* UploadBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&UploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&UploadResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&UploadBuff));

	// �V�F�[�_�[���\�[�X
	ID3D12Resource* TexBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&TexHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&TexResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&TexBuff));


	// GPU�Ƀ��\�[�X���A�b�v���[�h

	// ���_���̃A�b�v���[�h
	Vertex* VertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&VertMap);
	std::copy(std::begin(vertices), std::end(vertices), VertMap);
	vertBuff->Unmap(0, nullptr);

	// �C���f�b�N�X���̃A�b�v���[�h
	unsigned short* IndMap = nullptr;
	result = indBuff->Map(0, nullptr, (void**)&IndMap);
	std::copy(std::begin(indices), std::end(indices), IndMap);
	indBuff->Unmap(0, nullptr);

	// �A�b�v���[�h���\�[�X�փA�b�v���[�h
	uint8_t* ImgMap = nullptr;
	result = UploadBuff->Map(0, nullptr, (void**)&ImgMap);
	std::copy_n(LoadedImage->pixels, LoadedImage->slicePitch, ImgMap);
	UploadBuff->Unmap(0, nullptr);

	/*
	// �e�N�X�`���̃R�s�[WriteToSubresource��
	result = TexBuff->WriteToSubresource(0, nullptr, LoadedImage->pixels, (UINT)LoadedImage->rowPitch, (UINT)LoadedImage->slicePitch);
	DebugResultWithWindow(result, "Upload Texture Failed", "Error");
	*/


	// �e�N�X�`���̃R�s�[���A�R�s�[��̐ݒ�
	D3D12_TEXTURE_COPY_LOCATION TexSrc = {};
	TexSrc.pResource = UploadBuff;
	TexSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	TexSrc.PlacedFootprint.Offset = 0;
	TexSrc.PlacedFootprint.Footprint.Width = (UINT)LoadedImageMetaData.width;
	TexSrc.PlacedFootprint.Footprint.Height = (UINT)LoadedImageMetaData.height;
	TexSrc.PlacedFootprint.Footprint.Depth = (UINT)LoadedImageMetaData.depth;
	TexSrc.PlacedFootprint.Footprint.RowPitch = (UINT)LoadedImage->rowPitch;
	TexSrc.PlacedFootprint.Footprint.Format = LoadedImage->format;

	D3D12_TEXTURE_COPY_LOCATION TexDst = {};
	TexDst.pResource = TexBuff;
	TexDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	TexDst.SubresourceIndex = 0;


	// �e�N�X�`���p�f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* TexDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC TexDescHeapDesc = {};

	TexDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	TexDescHeapDesc.NodeMask = 0;
	TexDescHeapDesc.NumDescriptors = 1;
	TexDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(&TexDescHeapDesc, IID_PPV_ARGS(&TexDescHeap));
	DebugResultWithWindow(result, "CreateDescriptor Failed", "Error");


	// ���\�[�X�o�b�t�@�r���[(���C�A�E�g�Ƃ͈���āA�f�[�^��̏���ݒ�B�ǂ̕������ǂꂭ�炢�u����v����ݒ肷��B)

	// ���_�o�b�t�@�r���[���쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	// �C���f�b�N�X�o�b�t�@�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = indBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	// �V�F�[�_�[���\�[�X�r���[
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = LoadedImageMetaData.format;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	_dev->CreateShaderResourceView(TexBuff, &SRVDesc, TexDescHeap->GetCPUDescriptorHandleForHeapStart());


	// �V�F�[�_�[�̏���
	ID3DBlob* _vsBlob;
	ID3DBlob* _psBlob;
	ID3DBlob* errorBlob;
	string errStr;


	// ���_�V�F�[�_�[�̃R���p�C��
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&_vsBlob, &errorBlob);

	DebugCompileError(result, errorBlob, errStr, L"BasicVS");


	// �s�N�Z���V�F�[�_�[�̃R���p�C��
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&_psBlob, &errorBlob);

	DebugCompileError(result, errorBlob, errStr, L"BasicPS");


	// ���̓��C�A�E�g���쐬
	D3D12_INPUT_ELEMENT_DESC inputLayput[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};


	// �f�B�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTbRange = {};
	descTbRange.NumDescriptors = 1;
	descTbRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTbRange.BaseShaderRegister = 0;
	descTbRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ���[�g�p�����[�^�̍쐬 �f�B�X�N���v�^�e�[�u���Ƃ��ė��p
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParam.DescriptorTable.pDescriptorRanges = &descTbRange;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;


	// �T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;


	// ���[�g�V�O�l�`���̐ݒ�A�R���p�C���A����
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;
	rootSignatureDesc.NumParameters = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	DebugCompileError(result, errorBlob, errStr, L"RootSignature");

	ID3D12RootSignature* rootSignature = nullptr;
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "RootSignature Error");

	// �s�v�ɂȂ����u���u�����
	rootSigBlob->Release();
#pragma endregion

#pragma region GraphicsPipelineState
	// �O���t�B�b�N�X�p�C�v���C���̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

	// ���[�g�V�O�l�`���̐ݒ�
	pipelineDesc.pRootSignature = rootSignature;

	// �V�F�[�_�[���w��
	pipelineDesc.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = _vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = _psBlob->GetBufferSize();

	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.MultisampleEnable = false;
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.DepthClipEnable = true;

	// �u�����h�X�e�[�g�̐ݒ�
	pipelineDesc.BlendState.AlphaToCoverageEnable = false;
	pipelineDesc.BlendState.IndependentBlendEnable = false;

	// �����_�[�^�[�Q�b�g�̃u�����h�̎d����ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// �ݒ��K�p
	pipelineDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// �C���v�b�g���C�A�E�g
	pipelineDesc.InputLayout.pInputElementDescs = inputLayput;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayput);

	// �O�p�`�̃X�g���b�v�ݒ�
	pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// �v���~�e�B�u��ݒ�(�O�p�`)
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// �����_�[�^�[�Q�b�g�̈�ʐݒ�
	pipelineDesc.NumRenderTargets = 1;	// �g�p���郌���_�[�^�[�Q�b�g��1�̂�
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �A���`�G�C���A�V���O�ݒ�
	pipelineDesc.SampleDesc.Count = 1;		// 1�s�N�Z���ɂ�1��T���v�����O
	pipelineDesc.SampleDesc.Quality = 0;	// �Œ�ݒ�

	// �c��
	pipelineDesc.RasterizerState.FrontCounterClockwise = false;
	pipelineDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	pipelineDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pipelineDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pipelineDesc.RasterizerState.AntialiasedLineEnable = false;
	pipelineDesc.RasterizerState.ForcedSampleCount = 0;
	pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DepthStencilState.StencilEnable = false;


	// �O���t�B�b�N�X�p�C�v���C���I�u�W�F�N�g���쐬
	ID3D12PipelineState* _pipelineState = nullptr;
	result = _dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Pipeline State Error");
#pragma endregion

#pragma region Viewports
	// �r���[�|�[�g�̐ݒ�
	D3D12_VIEWPORT viewport = {};
	viewport.Width = WINDOW_WIDTH;
	viewport.Height = WINDOW_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;


	// �V�U�[��`�̐ݒ�
	D3D12_RECT scissorrect = {};
	scissorrect.left = 0;
	scissorrect.top = 0;
	scissorrect.right = scissorrect.left + WINDOW_WIDTH;
	scissorrect.bottom = scissorrect.top + WINDOW_HEIGHT;
#pragma endregion

#pragma region DirectXDraw
	// �o�b�N�o�b�t�@�̃C���f�b�N�X
	UINT bbIdx = _swapchain->GetCurrentBackBufferIndex();

	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += (size_t)bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// �o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER BarrierDesc = {};

	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = _backBuffers[bbIdx];
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12_RESOURCE_BARRIER BarrierDescTex = {};
	BarrierDescTex.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDescTex.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDescTex.Transition.pResource = TexBuff;
	BarrierDescTex.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDescTex.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	BarrierDescTex.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;


	// �o�b�N�o�b�t�@�ւ̃o���A�K�p
	_cmdList->ResourceBarrier(1, &BarrierDesc);


	// �����_�[�^�[�Q�b�g�Ƃ��Č������ݒ�
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);


	// ��ʃN���A�̐F
	float clearColor[] = { 1.f, 1.f, 0.f, 1.f };


	// ���߂̃X�g�b�N�J�n
	
	// �����_�[�^�[�Q�b�g��P�F�ŃN���A
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// �p�C�v���C���X�e�[�g���Z�b�g
	_cmdList->SetPipelineState(_pipelineState);

	// �r���[�A�V�U�[��`���Z�b�g
	_cmdList->RSSetViewports(1, &viewport);
	_cmdList->RSSetScissorRects(1, &scissorrect);

	// ���[�g�V�O�l�`�����Z�b�g
	_cmdList->SetGraphicsRootSignature(rootSignature);

	// �f�B�X�N���v�^�q�[�v�̎w��
	_cmdList->SetDescriptorHeaps(1, &TexDescHeap);

	// ���[�g�p�����[�^�ƃf�B�X�N���v�^�q�[�v�̊֘A�t��
	_cmdList->SetGraphicsRootDescriptorTable(0, TexDescHeap->GetGPUDescriptorHandleForHeapStart());

	// �e�N�X�`���̃Z�b�g
	_cmdList->ResourceBarrier(1, &BarrierDescTex);
	_cmdList->CopyTextureRegion(&TexDst, 0, 0, 0, &TexSrc, nullptr);

	// �e�N�X�`���o�b�t�@�ւ̃o���A�K�p
	BarrierDescTex.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	BarrierDescTex.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	_cmdList->ResourceBarrier(1, &BarrierDescTex);

	// �v���~�e�B�u�g�|���W�[���Z�b�g
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���_�o�b�t�@���Z�b�g
	_cmdList->IASetVertexBuffers(0, 1, &vbView);

	// �C���f�b�N�X�o�b�t�@���Z�b�g
	_cmdList->IASetIndexBuffer(&ibView);

	// �`�施��
	//_cmdList->DrawInstanced(4, 1, 0, 0);
	_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);


	// ��Ԃ��w��
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	_cmdList->ResourceBarrier(1, &BarrierDesc);


	// ���߂̃X�g�b�N�I��
	_cmdList->Close();


	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);


	// �t�F���X�ɂ��Ď�
	result = _cmdQueue->Signal(_fence, ++_fenceVal);
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Fence Signal Failed");

	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);

		_fence->SetEventOnCompletion(_fenceVal, event);

		WaitForSingleObject(event, INFINITE);

		CloseHandle(event);
	}


	// �L���[�̃��Z�b�g
	result = _cmdAllocator->Reset();
	DebugResultWithWindow(result, "E_FAILED2", "Commands Clear");


	// ���X�g�ɍēx���߂𗭂߂鏀��
	result = _cmdList->Reset(_cmdAllocator, nullptr);
	DebugResultWithWindow(result, "E_FAILED", "List Clear");


	// ��ʂ̃X���b�v(�o��)
	_swapchain->Present(1, 0);
#pragma endregion

	DebugResultWithWindow(result, "DX12 Succeeded!", "Congratulations!");

#pragma region MessageLoop
	// ���b�Z�[�W���[�v
	MSG msg = {};
	
	while (true)
	{
		// ���b�Z�[�W���󂯎��A�v���V�[�W���ɑ���
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		Sleep(1000);
	}
#pragma endregion

	iSimpleWindow->Release();

	return 0;
}


void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}