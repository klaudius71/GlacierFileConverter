#ifndef _VERTEX
#define _VERTEX

#include "Converter.h"

struct Vertex
{
	Vec3 pos{ 0.0f, 0.0f, 0.0f };
	Vec2 uv{ 0.0f, 0.0f };
	uint32_t tex_id = 0;
	Vec3 normal{ 0.0f, 1.0f, 0.0f };
	Vec3 tangent{ 0.0f, 0.0f, 1.0f };
	Vec3 bitangent{ 1.0f, 0.0f, 0.0f };
	uVec4 joint_ids{ 0, 0, 0, 0 };
	Vec4 joint_weights{ 1.0f, 1.0f, 1.0f, 1.0f };

	Vertex() = default;
	Vertex(const Vec3& pos, const Vec2& uv = Vec2{ 0.0f, 0.0f }, const uint32_t& tex_id = 0,
		const Vec3& normal = Vec3{ 0.0f, 1.0f, 0.0f }, const Vec3& tangent = Vec3{ 1.0f, 0.0f, 0.0f }, const Vec3& bitangent = Vec3{ 0.0f, 0.0f, 1.0f },
		const uVec4& joint_ids = uVec4{ 0, 0, 0, 0 }, const Vec4& joint_weights = Vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
		: pos(pos), uv(uv), tex_id(tex_id), normal(normal), tangent(tangent), bitangent(bitangent), joint_ids(joint_ids), joint_weights(joint_weights)
	{}

	void set(const Vec3& _pos, const Vec2& _uv, const Vec3& _normal)
	{
		pos = _pos;
		uv = _uv;
		normal = _normal;
	}
};

#endif