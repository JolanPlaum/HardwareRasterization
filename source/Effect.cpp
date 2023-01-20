//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
#include "pch.h"
#include "Effect.h"
#include "Texture.h"
#include <cassert>

using namespace dae;


//-----------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------
Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, bool usesTexture)
{
	//Load Effect
	m_pEffect = LoadEffect(pDevice, assetFile);

	
	//Load Techniques
	m_pTechniquePoint = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechniquePoint->IsValid())
		std::wcout << L"DefaultTechnique not valid\n";

	m_pTechniqueLinear = m_pEffect->GetTechniqueByName("LinearTechnique");
	if (!m_pTechniqueLinear->IsValid())
		std::wcout << L"LinearTechnique not valid\n";

	m_pTechniqueAnisotropic = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
	if (!m_pTechniqueAnisotropic->IsValid())
		std::wcout << L"AnisotropicTechnique not valid\n";

	m_pTechnique = m_pTechniquePoint;


	//Load Matrices
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"Matrix Variable gWorldViewProj not valid\n";

	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
		std::wcout << L"Matrix Variable gWorld not valid\n";

	m_pMatInvViewVariable = m_pEffect->GetVariableByName("gInvView")->AsMatrix();
	if (!m_pMatInvViewVariable->IsValid())
		std::wcout << L"Matrix Variable gInvView not valid\n";


	//Load Textures
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"Shader Resource gDiffuseMap Variable not valid\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"Shader Resource gNormalMap Variable not valid\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"Shader Resource gSpecularMap Variable not valid\n";

	m_pGlossMapVariable = m_pEffect->GetVariableByName("gGlossMap")->AsShaderResource();
	if (!m_pGlossMapVariable->IsValid())
		std::wcout << L"Shader Resource gGlossMap Variable not valid\n";


	//Create Vertex Layout
	uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC* vertexDesc{};
	if (usesTexture)
	{
		numElements = CreateTextureVertexLayout(vertexDesc);
	}
	else
	{
		numElements = CreateColorVertexLayout(vertexDesc);
	}


	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	const HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		assert(false);

	delete vertexDesc;
}


//-----------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------
Effect::~Effect()
{
	if (m_pDiffuseMapVariable) m_pDiffuseMapVariable->Release();

	if (m_pMatWorldViewProjVariable) m_pMatWorldViewProjVariable->Release();

	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pTechnique) m_pTechnique->Release();
	if (m_pEffect) m_pEffect->Release();
}


//-----------------------------------------------------------------
// Public Member Functions
//-----------------------------------------------------------------
ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

uint32_t Effect::CreateColorVertexLayout(D3D11_INPUT_ELEMENT_DESC*& vertexDesc)
{
	static constexpr uint32_t numElements{ 2 };
	vertexDesc = new D3D11_INPUT_ELEMENT_DESC[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	return numElements;
}

uint32_t Effect::CreateTextureVertexLayout(D3D11_INPUT_ELEMENT_DESC*& vertexDesc)
{
	static constexpr uint32_t numElements{ 4 };
	vertexDesc = new D3D11_INPUT_ELEMENT_DESC[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TANGENT";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	return numElements;
}

void Effect::ToggleTechnique()
{
	m_TechniqueType = TechniqueType(((int)m_TechniqueType + 1) % (int)TechniqueType::End);

	switch (m_TechniqueType)
	{
	case TechniqueType::Point:
		m_pTechnique = m_pTechniquePoint;
		std::wcout << L"POINT Sampler State active\n";
		break;
	case TechniqueType::Linear:
		m_pTechnique = m_pTechniqueLinear;
		std::wcout << L"LINEAR Sampler State active\n";
		break;
	case TechniqueType::Anisotropic:
		m_pTechnique = m_pTechniqueAnisotropic;
		std::wcout << L"ANISOTROPIC Sampler State active\n";
		break;
	}
}

void Effect::SetWorldViewProjectionMatrix(Matrix& pMatrix)
{
	if (m_pMatWorldViewProjVariable)
		m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&pMatrix));
	else
		std::wcout << L"SetWorldViewProjectionMatrix failed\n";
}

void Effect::SetWorldMatrix(Matrix& pMatrix)
{
	if (m_pMatWorldVariable)
		m_pMatWorldVariable->SetMatrix(reinterpret_cast<float*>(&pMatrix));
	else
		std::wcout << L"SetWorldMatrix failed\n";
}

void Effect::SetInverseViewMatrix(Matrix& pMatrix)
{
	if (m_pMatInvViewVariable)
		m_pMatInvViewVariable->SetMatrix(reinterpret_cast<float*>(&pMatrix));
	else
		std::wcout << L"SetInverseViewMatrix failed\n";
}

void Effect::SetDiffuseMap(Texture* pTexture)
{
	if (pTexture)
	{
		if (m_pDiffuseMapVariable)
		{
			const HRESULT result = m_pDiffuseMapVariable->SetResource(pTexture->GetResourceView());
			if (FAILED(result))
				assert(false);
		}
		else
			std::wcout << L"SetDiffuseMap failed\n";
	}
	else
	{
		std::wcout << L"SetDiffuseMap failed: nullptr given\n";
	}
}

void Effect::SetNormalMap(Texture* pTexture)
{
	if (pTexture)
	{
		if (m_pNormalMapVariable)
		{
			const HRESULT result = m_pNormalMapVariable->SetResource(pTexture->GetResourceView());
			if (FAILED(result))
				assert(false);
		}
		else
			std::wcout << L"SetNormalMap failed\n";
	}
	else
	{
		std::wcout << L"SetNormalMap failed: nullptr given\n";
	}
}

void Effect::SetSpecularMap(Texture* pTexture)
{
	if (pTexture)
	{
		if (m_pSpecularMapVariable)
		{
			const HRESULT result = m_pSpecularMapVariable->SetResource(pTexture->GetResourceView());
			if (FAILED(result))
				assert(false);
		}
		else
			std::wcout << L"SetSpecularMap failed\n";
	}
	else
	{
		std::wcout << L"SetSpecularMap failed: nullptr given\n";
	}
}

void Effect::SetGlossinessMap(Texture* pTexture)
{
	if (pTexture)
	{
		if (m_pGlossMapVariable)
		{
			const HRESULT result = m_pGlossMapVariable->SetResource(pTexture->GetResourceView());
			if (FAILED(result))
				assert(false);
		}
		else
			std::wcout << L"SetGlossinessMap failed\n";
	}
	else
	{
		std::wcout << L"SetGlossinessMap failed: nullptr given\n";
	}
}


//-----------------------------------------------------------------
// Private Member Functions
//-----------------------------------------------------------------

