//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "Effect.h"

using namespace dae;


//-----------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------
Scene::Scene()
	: Scene(Camera{})
{
}

Scene::Scene(const Camera& camera)
	: m_pCamera(new Camera(camera))
{
}


//-----------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------
Scene::~Scene()
{
	delete m_pCamera;

	for (Mesh* pMesh : m_Meshes)
	{
		delete pMesh;
	}
	m_Meshes.clear();
}


//-----------------------------------------------------------------
// Public Member Functions
//-----------------------------------------------------------------
void Scene::Update(const Timer* pTimer)
{
	//Update camera first since we need to retrieve data from it
	m_pCamera->Update(pTimer);

	//Calculate the WorldViewProjection matrix
	Matrix worldViewProj = m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
	Matrix invView = m_pCamera->GetInverseViewMatrix();

	//Update effects for all meshes
	for (Mesh* pMesh : m_Meshes)
	{
		Matrix world = pMesh->GetWorldMatrix();
		worldViewProj = world * worldViewProj;

		pMesh->GetEffect()->SetWorldViewProjectionMatrix(worldViewProj);
		pMesh->GetEffect()->SetWorldMatrix(world);
		pMesh->GetEffect()->SetInverseViewMatrix(invView);
	}
}

void Scene::Render(ID3D11DeviceContext* pDeviceContext)
{
	for (Mesh* pMesh : m_Meshes)
	{
		pMesh->Render(pDeviceContext);
	}
}

void Scene::ToggleSamplerState() const
{
	for (Mesh* pMesh : m_Meshes)
	{
		pMesh->ToggleSamplerState();
	}
}

Mesh* Scene::AddMesh(ID3D11Device* pDevice, const std::vector<Vertex_PosCol>& vertices, const std::vector<uint32_t>& indices)
{
	m_Meshes.emplace_back(new Mesh(pDevice, vertices, indices));
	return m_Meshes.back();
}

Mesh* Scene::AddMesh(ID3D11Device* pDevice, const std::vector<Vertex_PosTex>& vertices, const std::vector<uint32_t>& indices)
{
	m_Meshes.emplace_back(new Mesh(pDevice, vertices, indices));
	return m_Meshes.back();
}


//-----------------------------------------------------------------
// Private Member Functions
//-----------------------------------------------------------------

