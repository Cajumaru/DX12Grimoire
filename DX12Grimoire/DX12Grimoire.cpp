#include "Cajumaru.h"

#include "ObjectSimpleDrawer.h"

void EnableDebugLayer();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	HRESULT result;
	RESULTMESSAGE resmes;

	UPOINTER(SObjectManager, iObjectManager, 100);

	CObject* iObject = iObjectManager->CreateObject<CObject>();
	CObjectSimpleDrawer* iDrawer = iObjectManager->CreateObject<CObjectSimpleDrawer>(10);
	CObjectSimpleDrawer* iDrawer2 = iObjectManager->CreateObject<CObjectSimpleDrawer>(20);

	iObject->Tick(0.1f);
	iDrawer->Tick(0.2f);
	iDrawer->Tick(0.3f);

#pragma region WindowGenerate
	UPOINTER(SSimpleWindow, iSimpleWindow, hInstance, lpCmdLine, nCmdShow);

	iSimpleWindow->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
#pragma endregion

#ifdef _DEBUG
	EnableDebugLayer();
#endif // _DEBUG

#pragma region DX12Init
	UPOINTER(SDXManager, iDXManager);

	resmes = iDXManager->Initialize(iSimpleWindow->GetWindowHandle(), WINDOW_WIDTH, WINDOW_HEIGHT);
	DebugResultMessage(resmes);


	_dxgiFactory = iDXManager->GetDXGIFactory();
	_dev = iDXManager->GetDevice();
	_cmdAllocator = iDXManager ->GetCommandAllocator();
	_cmdList = iDXManager ->GetCommandList();
	_cmdQueue = iDXManager->GetCommandQueue();
	_swapchain = iDXManager->GetSwapchain();
	_fence = iDXManager->GetFence();
	_fenceVal = iDXManager->GetFenceValue();
	rtvHeaps = iDXManager->GetRTVDescriptorHeap();
	_backBuffers = iDXManager->GetBackBuffers();
#pragma endregion

	iSimpleWindow->Show_Window();

#pragma region SetUpDrawObjectData
	// 頂点情報
	vector<Vertex> vertices = {
		{vec3(-1.f, -1.f, 0.f),	vec2(0.f, 1.f)},	// 左下
		{vec3(-1.f, 1.f, 0.f) , vec2(0.f, 0.f)},	// 左上
		{vec3(1.f, -1.f, 0.f) , vec2(1.f, 1.f)},	// 右下
		{vec3(1.f, 1.f, 0.f)  , vec2(1.f, 0.f)}		// 右上
	};


	// インデックスデータ
	vector<Index> indices = {
	0, 1, 2,
	2, 1, 3
	};


	DirectX::ScratchImage LoadedImageCache = {};
	DirectX::TexMetadata LoadedImageMetaData = {};

	result = DirectX::LoadFromWICFile(L"Natsu1.jpg", DirectX::WIC_FLAGS_NONE, &LoadedImageMetaData, LoadedImageCache);

	TextureNatsu.Image = LoadedImageCache.GetImage(0, 0, 0);
	TextureNatsu.Metadata = LoadedImageMetaData;

	const DirectX::Image* LoadedImage = TextureNatsu.Image;


	// データをオブジェクトに渡す
	iObject->SetVertices(&vertices);
	iObject->SetIndices(&indices);


	// ヒープ領域の設定
	D3D12_HEAP_PROPERTIES HeapProp = DX::HEAP_PROPERTIES_DEFAULT_BUFFER;

	// 中間バッファ(アップロードヒープ)用
	D3D12_HEAP_PROPERTIES UploadHeapProp = DX::HEAP_PROPERTIES_UPLOAD_BUFFER;

	// テクスチャのアップロード用
	D3D12_HEAP_PROPERTIES TexHeapProp = DX::HEAP_PROPERTIES_TEXTURE_BUFFER;


	// リソース設定構造体
	D3D12_RESOURCE_DESC ResDesc = {};
	ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResDesc.Width = sizeof(vertices[0]) * vertices.size();
	ResDesc.Height = 1;
	ResDesc.DepthOrArraySize = 1;
	ResDesc.MipLevels = 1;
	ResDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResDesc.SampleDesc.Count = 1;
	ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// アップロード用
	D3D12_RESOURCE_DESC UploadResDesc = {};
	UploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	UploadResDesc.Width = AlignmentedSize(LoadedImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * LoadedImageMetaData.height;
	UploadResDesc.Height = 1;
	UploadResDesc.Format = DXGI_FORMAT_UNKNOWN;
	UploadResDesc.DepthOrArraySize = 1;
	UploadResDesc.MipLevels = 1;
	UploadResDesc.SampleDesc.Count = 1;
	UploadResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	UploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// テクスチャ用の設定
	D3D12_RESOURCE_DESC TexResDesc = {};
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


	// リソース領域の確保

	// 頂点バッファ
	ID3D12Resource* vertBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&DX::HEAP_PROPERTIES_DEFAULT_BUFFER,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// インデックスバッファ
	ResDesc.Width = sizeof(indices[0]) * indices.size();

	ID3D12Resource* indBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&DX::HEAP_PROPERTIES_DEFAULT_BUFFER,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indBuff));

	// アップロードバッファ
	ID3D12Resource* UploadBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&DX::HEAP_PROPERTIES_DEFAULT_BUFFER,
		D3D12_HEAP_FLAG_NONE,
		&UploadResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&UploadBuff));

	// シェーダーリソース
	ID3D12Resource* TexBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&DX::HEAP_PROPERTIES_TEXTURE_BUFFER,
		D3D12_HEAP_FLAG_NONE,
		&TexResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&TexBuff));


	// GPUにリソースをアップロード

	// 頂点情報のアップロード
	Vertex* VertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&VertMap);
	std::copy(vertices.begin(), vertices.end(), VertMap);
	vertBuff->Unmap(0, nullptr);

	// インデックス情報のアップロード
	unsigned short* IndMap = nullptr;
	result = indBuff->Map(0, nullptr, (void**)&IndMap);
	std::copy(indices.begin(), indices.end(), IndMap);	
	indBuff->Unmap(0, nullptr);


	// アップロードリソースへアップロード
	uint8_t* ImgMap = nullptr;
	result = UploadBuff->Map(0, nullptr, (void**)&ImgMap);
	
	uint8_t* SrcImgMap = LoadedImage->pixels;
	size_t RowPitch = AlignmentedSize(LoadedImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	for (int y = 0; y < LoadedImage->height; ++y)
	{
		std::copy_n(SrcImgMap, RowPitch, ImgMap);

		ImgMap += RowPitch;
		SrcImgMap += LoadedImage->rowPitch;
	}
	UploadBuff->Unmap(0, nullptr);


	// テクスチャのコピー元、コピー先の設定
	D3D12_TEXTURE_COPY_LOCATION TexSrc = {};
	TexSrc.pResource = UploadBuff;
	TexSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	TexSrc.PlacedFootprint.Offset = 0;
	TexSrc.PlacedFootprint.Footprint.Width = (UINT)LoadedImageMetaData.width;
	TexSrc.PlacedFootprint.Footprint.Height = (UINT)LoadedImageMetaData.height;
	TexSrc.PlacedFootprint.Footprint.Depth = (UINT)LoadedImageMetaData.depth;
	TexSrc.PlacedFootprint.Footprint.RowPitch = (UINT)AlignmentedSize(LoadedImage->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	TexSrc.PlacedFootprint.Footprint.Format = LoadedImage->format;

	D3D12_TEXTURE_COPY_LOCATION TexDst = {};
	TexDst.pResource = TexBuff;
	TexDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	TexDst.SubresourceIndex = 0;


	// テクスチャ用ディスクリプタヒープ
	ID3D12DescriptorHeap* TexDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC TexDescHeapDesc = {};

	TexDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	TexDescHeapDesc.NodeMask = 0;
	TexDescHeapDesc.NumDescriptors = 1;
	TexDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(&TexDescHeapDesc, IID_PPV_ARGS(&TexDescHeap));
	DebugResultWithWindow(result, "CreateDescriptor Failed", "Error");


	// リソースバッファビュー(レイアウトとは違って、データ塊の情報を設定。どの部分をどれくらい「見る」かを設定する。)

	// 頂点バッファビューを作成
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = (UINT)(sizeof(vertices[0]) * vertices.size());
	vbView.StrideInBytes = sizeof(vertices[0]);

	// インデックスバッファビューを作成
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = indBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = (UINT)(sizeof(indices[0]) * indices.size());

	// シェーダーリソースビュー
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = LoadedImageMetaData.format;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	_dev->CreateShaderResourceView(TexBuff, &SRVDesc, TexDescHeap->GetCPUDescriptorHandleForHeapStart());


	// シェーダーの準備
	ID3DBlob* _vsBlob;
	ID3DBlob* _psBlob;
	ID3DBlob* errorBlob;
	string errStr;


	// 頂点シェーダーのコンパイル
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&_vsBlob, &errorBlob);

	DebugCompileError(result, errorBlob, errStr, L"BasicVS");


	// ピクセルシェーダーのコンパイル
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&_psBlob, &errorBlob);

	DebugCompileError(result, errorBlob, errStr, L"BasicPS");


	// 入力レイアウトを作成
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


	// ディスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descTbRange = {};
	descTbRange.NumDescriptors = 1;
	descTbRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTbRange.BaseShaderRegister = 0;
	descTbRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ルートパラメータの作成 ディスクリプタテーブルとして利用
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParam.DescriptorTable.pDescriptorRanges = &descTbRange;
	rootParam.DescriptorTable.NumDescriptorRanges = 1;


	// サンプラーの設定
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


	// ルートシグネチャの設定、コンパイル、生成
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

	// 不要になったブロブを解放
	rootSigBlob->Release();

#pragma endregion

#pragma region GraphicsPipelineState
	// グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

	// ルートシグネチャの設定
	pipelineDesc.pRootSignature = rootSignature;

	// シェーダーを指定
	pipelineDesc.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = _vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = _psBlob->GetBufferSize();

	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ラスタライザの設定
	pipelineDesc.RasterizerState.MultisampleEnable = false;
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.DepthClipEnable = true;

	// ブレンドステートの設定
	pipelineDesc.BlendState.AlphaToCoverageEnable = true;
	pipelineDesc.BlendState.IndependentBlendEnable = false;

	// レンダーターゲットのブレンドの仕方を設定
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 設定を適用
	pipelineDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// インプットレイアウト
	pipelineDesc.InputLayout.pInputElementDescs = inputLayput;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayput);

	// 三角形のストリップ設定
	pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// プリミティブを設定(三角形)
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// レンダーターゲットの一般設定
	pipelineDesc.NumRenderTargets = 1;	// 使用するレンダーターゲットは1つのみ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// アンチエイリアシング設定
	pipelineDesc.SampleDesc.Count = 1;		// 1ピクセルにつき1回サンプリング
	pipelineDesc.SampleDesc.Quality = 0;	// 最低設定

	// 残り
	pipelineDesc.RasterizerState.FrontCounterClockwise = false;
	pipelineDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	pipelineDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pipelineDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pipelineDesc.RasterizerState.AntialiasedLineEnable = false;
	pipelineDesc.RasterizerState.ForcedSampleCount = 0;
	pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DepthStencilState.StencilEnable = false;


	// グラフィックスパイプラインオブジェクトを作成
	ID3D12PipelineState* _pipelineState = nullptr;
	result = _dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Pipeline State Error");
#pragma endregion

#pragma region Viewports
	// ビューポートの設定
	D3D12_VIEWPORT viewport = {};
	viewport.Width = WINDOW_WIDTH;
	viewport.Height = WINDOW_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;


	// シザー矩形の設定
	D3D12_RECT scissorrect = {};
	scissorrect.left = 0;
	scissorrect.top = 0;
	scissorrect.right = scissorrect.left + WINDOW_WIDTH;
	scissorrect.bottom = scissorrect.top + WINDOW_HEIGHT;
#pragma endregion

#pragma region DirectXDraw
	// バックバッファのインデックス
	UINT bbIdx = _swapchain->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += (size_t)bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// バリアの設定
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


	// バックバッファへのバリア適用
	_cmdList->ResourceBarrier(1, &BarrierDesc);


	// レンダーターゲットとして見る方を設定
	_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);


	// 画面クリアの色
	float clearColor[] = { 1.f, 1.f, 0.f, 1.f };


	// 命令のストック開始
	
	// レンダーターゲットを単色でクリア
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// パイプラインステートをセット
	_cmdList->SetPipelineState(_pipelineState);

	// ビュー、シザー矩形をセット
	_cmdList->RSSetViewports(1, &viewport);
	_cmdList->RSSetScissorRects(1, &scissorrect);

	// ルートシグネチャをセット
	_cmdList->SetGraphicsRootSignature(rootSignature);

	// ディスクリプタヒープの指定
	_cmdList->SetDescriptorHeaps(1, &TexDescHeap);

	// ルートパラメータとディスクリプタヒープの関連付け
	_cmdList->SetGraphicsRootDescriptorTable(0, TexDescHeap->GetGPUDescriptorHandleForHeapStart());

	// テクスチャのセット
	_cmdList->ResourceBarrier(1, &BarrierDescTex);
	_cmdList->CopyTextureRegion(&TexDst, 0, 0, 0, &TexSrc, nullptr);

	// テクスチャバッファへのバリア適用
	BarrierDescTex.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	BarrierDescTex.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	_cmdList->ResourceBarrier(1, &BarrierDescTex);

	// プリミティブトポロジーをセット
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファをセット
	_cmdList->IASetVertexBuffers(0, 1, &vbView);

	// インデックスバッファをセット
	_cmdList->IASetIndexBuffer(&ibView);

	// 描画命令
	_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);


	// 状態を指定
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	_cmdList->ResourceBarrier(1, &BarrierDesc);


	// 命令のストック終了
	_cmdList->Close();


	// コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);


	// フェンスによる監視
	result = _cmdQueue->Signal(_fence, ++_fenceVal);
	DebugResultWithWindow(result, "Use Latest Graphics Card", "Fence Signal Failed");

	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);

		_fence->SetEventOnCompletion(_fenceVal, event);

		WaitForSingleObject(event, INFINITE);

		CloseHandle(event);
	}


	// キューのリセット
	result = _cmdAllocator->Reset();
	DebugResultWithWindow(result, "E_FAILED2", "Commands Clear");


	// リストに再度命令を溜める準備
	result = _cmdList->Reset(_cmdAllocator, nullptr);
	DebugResultWithWindow(result, "E_FAILED", "List Clear");


	// 画面のスワップ(出力)
	_swapchain->Present(1, 0);
#pragma endregion

	DebugResultWithWindow(result, "DX12 Succeeded!", "Congratulations!");

#pragma region MessageLoop
	// メッセージループ
	MSG msg = {};
	
	while (true)
	{
		// メッセージを受け取り、プロシージャに送る
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
	iSimpleWindow.release();

	iDXManager.release();

	iObjectManager->ClearAll();
	iObjectManager.release();

	return 0;
}


void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}