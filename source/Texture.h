#pragma once
// Includes

namespace dae
{
	// Class Forward Declarations
	
	// Class Declaration
	class Texture final
	{
	public:
		// Constructors and Destructor
		explicit Texture(ID3D11Device* pDevice, const std::string& path);
		~Texture();
		
		// Copy and Move semantics
		Texture(const Texture& other)					= delete;
		Texture& operator=(const Texture& other)		= delete;
		Texture(Texture&& other) noexcept				= delete;
		Texture& operator=(Texture&& other) noexcept	= delete;
	
		//---------------------------
		// Public Member Functions
		//---------------------------
		ID3D11ShaderResourceView* GetResourceView() const { return m_pSRV; }

		
	private:
		// Member variables
		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pSRV{};
	
		//---------------------------
		// Private Member Functions
		//---------------------------
	
	};
}
