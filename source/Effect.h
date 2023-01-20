#pragma once
// Includes

namespace dae
{
	// Forward Declarations
	class Texture;
	
	// Class Declaration
	class Effect final
	{
	public:
		// Constructors and Destructor
		explicit Effect(ID3D11Device* pDevice, const std::wstring& assetFile, bool usesTexture);
		~Effect();

		// Copy and Move semantics
		Effect(const Effect& other)					= delete;
		Effect& operator=(const Effect& other)		= delete;
		Effect(Effect&& other) noexcept				= delete;
		Effect& operator=(Effect&& other) noexcept	= delete;

		//---------------------------
		// Public Member Functions
		//---------------------------
		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		uint32_t CreateColorVertexLayout(D3D11_INPUT_ELEMENT_DESC*& vertexDesc);
		uint32_t CreateTextureVertexLayout(D3D11_INPUT_ELEMENT_DESC*& vertexDesc);

		void ToggleTechnique();

		void SetWorldViewProjectionMatrix(Matrix& pMatrix);
		void SetWorldMatrix(Matrix& pMatrix);
		void SetInverseViewMatrix(Matrix& pMatrix);

		void SetDiffuseMap(Texture* pTexture);
		void SetNormalMap(Texture* pTexture);
		void SetSpecularMap(Texture* pTexture);
		void SetGlossinessMap(Texture* pTexture);

		ID3DX11Effect* GetEffect() const { return m_pEffect; }
		ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }


	private:
		// Member variables
		ID3DX11Effect* m_pEffect{};
		ID3D11InputLayout* m_pInputLayout{};

		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
		ID3DX11EffectMatrixVariable* m_pMatWorldVariable{};
		ID3DX11EffectMatrixVariable* m_pMatInvViewVariable{};

		ID3DX11EffectTechnique* m_pTechnique{};
		ID3DX11EffectTechnique* m_pTechniquePoint{};
		ID3DX11EffectTechnique* m_pTechniqueLinear{};
		ID3DX11EffectTechnique* m_pTechniqueAnisotropic{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable{};

		enum class TechniqueType
		{
			Point,
			Linear,
			Anisotropic,

			//@END
			End
		} m_TechniqueType;

		//---------------------------
		// Private Member Functions
		//---------------------------

	};
}
