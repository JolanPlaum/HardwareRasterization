#include "pch.h"
#include "Renderer.h"
#include "Utils.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "Effect.h"
#include "Texture.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		IDXGIFactory1* pDxgiFactory{};
		const HRESULT result = InitializeDirectX(pDxgiFactory);
		if (pDxgiFactory) pDxgiFactory->Release();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";

			m_pScene = Scene_5();
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
	}

	Renderer::~Renderer()
	{
		if (m_pScene) delete m_pScene;

		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();

		if (m_pDepthStencilView) m_pDepthStencilView->Release();
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();

		if (m_pSwapChain) m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice) m_pDevice->Release();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pScene->Update(pTimer);

		if (m_pMeshRotating)
		{
			m_pMeshRotating->Rotate({ 0.f, pTimer->GetElapsed() * PI_DIV_2, 0.f });
		}
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. CLEAR RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0.f, 0.f, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		m_pScene->Render(m_pDeviceContext);

		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::ToggleSamplerStates() const
	{
		m_pScene->ToggleSamplerState();
	}

	HRESULT Renderer::InitializeDirectX(IDXGIFactory1*& pDxgiFactory)
	{
		//1. Create Device & Context
		//=====
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(	nullptr,
											D3D_DRIVER_TYPE_HARDWARE,
											0,
											createDeviceFlags,
											&featureLevel,
											1,
											D3D11_SDK_VERSION,
											&m_pDevice,
											nullptr,
											&m_pDeviceContext);
		if (FAILED(result))
			return result;

		//Create DXGI Factory
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;



		//2. Create Swapchain
		//=====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//Get the handle (HWND) from the SDL Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Creat Swapchain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;



		//3. Create DepthStencil (DS) and DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;



		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;



		//5. Bind RTV & DSV to Output Merger Stage
		//=====
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);



		//6. Set Viewport
		//=====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);



		return result;
	}

#pragma region SCENES
	Scene* Renderer::Scene_1()
	{
		//Instantiate the scene
		Scene* scene = new Scene(Camera({ 0.f,0.f,-10.f }, 45.f, m_Width / (float)m_Height));

		//Create some data for our mesh
		std::vector<Vertex_PosCol> vertices{
			{{  0.f,  3.f, 2.f },		{ 1.f, 0.f, 0.f }},
			{{  3.f, -3.f, 2.f },		{ 0.f, 0.f, 1.f }},
			{{ -3.f, -3.f, 2.f },		{ 0.f, 1.f, 0.f }},
		};
		std::vector<uint32_t> indices{ 0,1,2 };

		//Add mesh to the scene
		scene->AddMesh(new Mesh(m_pDevice, L"Resources/PosCol3D.fx", vertices, indices));

		return scene;
	}

	Scene* Renderer::Scene_2()
	{
		//Instantiate the scene
		Scene* scene = new Scene(Camera({ 0.f,0.f,-10.f }, 45.f, m_Width / (float)m_Height));

		//Create some data for our mesh
		std::vector<Vertex_PosTex> vertices{
				{ {	-3,	3,	-2 }, {}, {}, { 0.f,	0.f } },
				{ {	0,	3,	-2 }, {}, {}, { .5f,	0.f } },
				{ { 3,	3,	-2 }, {}, {}, { 1.f,	0.f } },
				{ {	-3,	0,	-2 }, {}, {}, { 0.f,	.5f } },
				{ { 0,	0,	-2 }, {}, {}, { .5f,	.5f } },
				{ { 3,	0,	-2 }, {}, {}, { 1.f,	.5f } },
				{ {	-3,	-3,	-2 }, {}, {}, { 0.f,	1.f } },
				{ { 0,	-3,	-2 }, {}, {}, { .5f,	1.f } },
				{ { 3,	-3,	-2 }, {}, {}, { 1.f,	1.f } }
		};
		std::vector<uint32_t>indices{
			3, 0, 1,	1, 4, 3,	4, 1, 2,
			2, 5, 4,	6, 3, 4,	4, 7, 6,
			7, 4, 5,	5, 8, 7
		};

		//Add mesh to the scene
		Mesh* pMesh = new Mesh(m_pDevice, L"Resources/PosTex3D.fx", vertices, indices);
		pMesh->SetDiffuseTexture(new Texture(m_pDevice, "Resources/uv_grid_2.png"));

		scene->AddMesh(pMesh);

		return scene;
	}

	Scene* Renderer::Scene_3()
	{
		//Instantiate the scene
		Scene* scene = new Scene(Camera({ 0.f, 0.f, -50.f }, 45.f, m_Width / (float)m_Height));

		//Create data for our mesh
		std::vector<Vertex_PosTex> vertices{};
		std::vector<uint32_t> indices{};
		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);

		//Add mesh to the scene
		m_pMeshRotating = new Mesh(m_pDevice, L"Resources/PosTex3D.fx", vertices, indices);
		m_pMeshRotating->SetDiffuseTexture(new Texture(m_pDevice, "Resources/vehicle_diffuse.png"));

		scene->AddMesh(m_pMeshRotating);

		return scene;
	}

	Scene* Renderer::Scene_4()
	{
		//Instantiate the scene
		Scene* scene = new Scene(Camera({ 0.f, 0.f, -50.f }, 45.f, m_Width / (float)m_Height));

		//Create data for our mesh
		std::vector<Vertex_PosTex> vertices{};
		std::vector<uint32_t> indices{};
		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);

		//Add mesh to the scene
		m_pMeshRotating = new Mesh(m_pDevice, L"Resources/PosTex3D.fx", vertices, indices);
		m_pMeshRotating->SetDiffuseTexture(new Texture(m_pDevice, "Resources/vehicle_diffuse.png"));
		m_pMeshRotating->SetNormalTexture(new Texture(m_pDevice, "Resources/vehicle_normal.png"));
		m_pMeshRotating->SetSpecularTexture(new Texture(m_pDevice, "Resources/vehicle_specular.png"));
		m_pMeshRotating->SetGlossinessTexture(new Texture(m_pDevice, "Resources/vehicle_gloss.png"));

		scene->AddMesh(m_pMeshRotating);

		return scene;
	}

	Scene* Renderer::Scene_5()
	{
		//Instantiate the scene
		Scene* scene = new Scene(Camera({ 0.f, 0.f, -50.f }, 45.f, m_Width / (float)m_Height));

		
		//Create data for our vehicle mesh
		std::vector<Vertex_PosTex> vertices{};
		std::vector<uint32_t> indices{};
		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);

		//Add vehicle mesh to the scene
		Mesh* pMesh = new Mesh(m_pDevice, L"Resources/PosTex3D.fx", vertices, indices);
		pMesh->SetDiffuseTexture(new Texture(m_pDevice, "Resources/vehicle_diffuse.png"));
		pMesh->SetNormalTexture(new Texture(m_pDevice, "Resources/vehicle_normal.png"));
		pMesh->SetSpecularTexture(new Texture(m_pDevice, "Resources/vehicle_specular.png"));
		pMesh->SetGlossinessTexture(new Texture(m_pDevice, "Resources/vehicle_gloss.png"));

		scene->AddMesh(pMesh);


		//Create data for our fire mesh
		Utils::ParseOBJ("Resources/fireFX.obj", vertices, indices);

		//Add mesh to the scene
		pMesh = new Mesh(m_pDevice, L"Resources/PosDiffuse3D.fx", vertices, indices);
		pMesh->SetDiffuseTexture(new Texture(m_pDevice, "Resources/fireFX_diffuse.png"));

		scene->AddMesh(pMesh);


		return scene;
	}
#pragma endregion
}
