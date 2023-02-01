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

	Vertex(const float& pos_x = 0.0f, const float& pos_y = 0.0f, const float& pos_z = 0.0f,
		const float& u = 0.0f, const float& v = 0.0f, const uint32_t& tex_id = 0,
		const float& normal_x = 0.0f, const float& normal_y = 1.0f, const float& normal_z = 0.0f,
		const float& tangent_x = 1.0f, const float& tangent_y = 0.0f, const float& tangent_z = 0.0f,
		const float& bitangent_x = 0.0f, const float& bitangent_y = 0.0f, const float& bitangent_z = 1.0f,
		const uint32_t& joints_ids0 = 0, const uint32_t& joints_ids1 = 0, const uint32_t& joints_ids2 = 0, const uint32_t& joints_ids3 = 0,
		const float& joint_weights0 = 1.0f, const float& joint_weights1 = 1.0f, const float& joint_weights2 = 1.0f, const float& joint_weights3 = 1.0f)
		: pos(pos_x, pos_y, pos_z), uv(u, v), tex_id(tex_id),
		normal(normal_x, normal_y, normal_z), tangent(tangent_x, tangent_y, tangent_z), bitangent(bitangent_x, bitangent_y, bitangent_z),
		joint_ids(joints_ids0, joints_ids1, joints_ids2, joints_ids3), joint_weights(joint_weights0, joint_weights1, joint_weights2, joint_weights3)
	{}
	Vertex(const Vec3& pos, const Vec2& uv = Vec2(0.0f, 0.0f), const uint32_t& tex_id = 0,
		const Vec3& normal = Vec3(0.0f, 1.0f, 0.0f), const Vec3& tangent = Vec3(1.0f, 0.0f, 0.0f), const Vec3& bitangent = Vec3(0.0f, 0.0f, 1.0f),
		const uVec4& joint_ids = uVec4(0, 0, 0, 0), const Vec4& joint_weights = Vec4(1.0f, 1.0f, 1.0f, 1.0f))
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