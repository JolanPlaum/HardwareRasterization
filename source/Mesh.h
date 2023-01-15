#pragma once
// Includes
#include "DataTypes.h"

namespace dae
{
	class Effect;

	// Class Declaration
	class Mesh final
	{
	public:
		// Constructors and Destructor
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		// Copy and Move semantics
		Mesh(const Mesh& other)					= delete;
		Mesh& operator=(const Mesh& other)		= delete;
		Mesh(Mesh&& other) noexcept				= delete;
		Mesh& operator=(Mesh&& other) noexcept	= delete;

		//---------------------------
		// Public Member Functions
		//---------------------------
		void Render(ID3D11DeviceContext* pDeviceContext);


	private:
		// Member variables
		Effect* m_pEffect{};

		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};

		uint32_t m_NumIndices{};

		//---------------------------
		// Private Member Functions
		//---------------------------

	};
}
