#pragma once
// Includes

namespace dae
{
	// Forward Declarations
	
	// Class Declaration
	class Effect final
	{
	public:
		// Constructors and Destructor
		explicit Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
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

		ID3DX11Effect* GetEffect() { return m_pEffect; }
		ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; }
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }


	private:
		// Member variables
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};

		ID3D11InputLayout* m_pInputLayout{};

		//---------------------------
		// Private Member Functions
		//---------------------------

	};
}
