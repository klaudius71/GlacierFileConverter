#ifndef _CONVERTER
#define _CONVERTER

#include <tuple>
#include <string>

struct GlacierFileHdr;

using Vec2 = std::tuple<float, float>;
using Vec3 = std::tuple<float, float, float>;
using Vec4 = std::tuple<float, float, float, float>;
using Quat = std::tuple<float, float, float, float>;
using uVec4 = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>;
using Mat4 = std::tuple<float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float>;

#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_INT 0x1405

namespace Converter
{
	void Convert(const char* const file_name);
	void WriteDataToFile(const std::string& file_name, const GlacierFileHdr& data);
};

#endif