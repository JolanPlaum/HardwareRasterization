#pragma once
// Includes
#include "DataTypes.h"

namespace dae
{
	// Forward Declarations
	class Effect;
	class Texture;

	// Class Declaration
	class Mesh final
	{
	public:
		// Constructors and Destructor
		explicit Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices);
		explicit Mesh(ID3D11Device* pDevice, const std::vector<Vertex_PosTex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		// Copy and Move semantics
		Mesh(const Mesh& other)					= delete;
		Mesh& operator=(const Mesh& other)		= delete;
		Mesh(Mesh&& other) noexcept				= delete;
		Mesh& operator=(Mesh&& other) noexcept	= delete;

		//---------------------------
		// Public Member Functions
		//---------------------------
		void Render(ID3D11DeviceContext* pDeviceContext) const;

		void ToggleSamplerState() const;
		void Translate(const Vector3& translation);
		void Rotate(const Vector3& rotation);

		void SetDiffuseTexture(Texture* pTexture);
		void SetNormalTexture(Texture* pTexture);
		void SetSpecularTexture(Texture* pTexture);
		void SetGlossinessTexture(Texture* pTexture);
		void SetPosition(float x, float y, float z);
		void SetRotation(float pitch, float yaw, float roll);
		void SetScale(const Vector3& scale);

		Effect* GetEffect() const { return m_pEffect; }
		Matrix GetWorldMatrix() const { return Matrix::CreateTransform(m_Position, m_Rotation, m_Scale); }


	private:
		// Member variables
		bool m_IsTextured{};
		Effect* m_pEffect{};

		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};

		uint32_t m_NumIndices{};

		Texture* m_pDiffuseTexture{};
		Texture* m_pNormalTexture{};
		Texture* m_pSpecularTexture{};
		Texture* m_pGlossTexture{};

		Vector3 m_Position{ 0.f, 0.f, 0.f };
		Vector3 m_Rotation{ 0.f, 0.f, 0.f };
		Vector3 m_Scale{ 1.f, 1.f, 1.f };

		//---------------------------
		// Private Member Functions
		//---------------------------

	};
}
