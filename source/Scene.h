#pragma once
// Includes
#include "DataTypes.h"

namespace dae
{
	// Forward Declarations
	class Camera;
	class Mesh;
	
	// Class Declaration
	class Scene final
	{
	public:
		// Constructors and Destructor
		explicit Scene();
		explicit Scene(const Camera& camera);
		~Scene();
		
		// Copy and Move semantics
		Scene(const Scene& other)					= delete;
		Scene& operator=(const Scene& other)		= delete;
		Scene(Scene&& other) noexcept				= delete;
		Scene& operator=(Scene&& other) noexcept	= delete;
	
		//---------------------------
		// Public Member Functions
		//---------------------------
		void Update(const Timer* pTimer);
		void Render(ID3D11DeviceContext* pDeviceContext);

		void AddMesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices);
	
	
	private:
		// Member variables
		Camera* m_pCamera{};

		std::vector<Mesh*> m_Meshes{};
	
		//---------------------------
		// Private Member Functions
		//---------------------------
	
	};
}