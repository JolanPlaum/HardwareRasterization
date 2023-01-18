#pragma once

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
	};

	struct Vertex_PosTex
	{
		Vector3 position{};
		Vector2 uv{};
	};
}