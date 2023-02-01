#ifndef _CONVERTER
#define _CONVERTER

#include <tuple>
#include <string>

struct GlacierFileHdr;

struct Vec2
{
	float x;
	float y;
};
struct Vec3
{
	float x;
	float y;
	float z;
};
struct Vec4
{
	float x;
	float y;
	float z;
	float w;
};
struct uVec4
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;

	uVec4(const uint32_t& _x, const uint32_t& _y, const uint32_t& _z, const uint32_t& _w)
		: x(_x), y(_y), z(_z), w(_w)
	{}
};
struct Mat4
{
	float m00;
	float m01;
	float m02;
	float m03;
	float m10;
	float m11;
	float m12;
	float m13;
	float m20;
	float m21;
	float m22;
	float m23;
	float m30;
	float m31;
	float m32;
	float m33;
};

#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405

namespace Converter
{
	void Convert(const char* const file_name);
	void WriteDataToFile(const std::string& file_name, const GlacierFileHdr& data);
};

#endif