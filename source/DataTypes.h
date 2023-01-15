#pragma once

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
	};

	struct Vertex_PosCol_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
	};
}