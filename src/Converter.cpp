#include "Converter.h"
#include "GlacierFileHdr.h"
#include "Vertex.h"
#include <fstream>
#include "json.hpp"
#include "base64.h"

namespace Converter {

	void Convert(const char* const file_name)
	{
		// Parsing the glTF file, which is just JSON
		std::ifstream file(file_name);
		if (!file.is_open())
		{
			printf("File not found!\n");
			exit(-1);
		}
		printf("Parsing %s...\n", file_name);
		const nlohmann::json gltf = std::move(nlohmann::json::parse(file));
		printf("Done.\n");
		file.close();

		// Grabbing the raw data
		const std::string& uri = gltf["buffers"][0]["uri"];
		const std::vector<uint8_t> buffer_data = base64_decode_bytes(uri.substr(uri.find(',') + 1));
		assert(buffer_data.size() == gltf["buffers"][0]["byteLength"]);

		struct BufferData
		{
			uint32_t stride;
			uint32_t count;
			const void* data;

			BufferData(uint32_t stride, uint32_t count, const void* data)
				: stride(stride), count(count), data(data) {}
		};
		std::map<std::string, BufferData> data;

		GlacierFileHdr hdr;

		// Load mesh data
		assert(gltf.find("meshes") != gltf.cend());
		const char* atts[] = { "POSITION", "NORMAL", "TEXCOORD_0", "JOINTS_0", "WEIGHTS_0" };
		const auto& primitives = gltf["meshes"][0]["primitives"][0];
		const auto& attributes = primitives["attributes"];
		for (auto att : atts)
		{
			auto it = attributes.find(att);
			if (it != attributes.end())
			{
				const uint32_t accessor_index = it.value();
				const auto& accessor = gltf["accessors"][accessor_index];
				const auto& bufferView = gltf["bufferViews"][accessor_index];
				const uint8_t* buff = buffer_data.data() + (uint32_t)bufferView["byteOffset"];
				const std::string type = accessor["type"];
				const uint32_t count = accessor["count"];
				if (type == "VEC3")
					data.emplace(std::piecewise_construct, std::forward_as_tuple(att), std::forward_as_tuple(4, count * 3, buff));
				else if (type == "VEC2")
					data.emplace(std::piecewise_construct, std::forward_as_tuple(att), std::forward_as_tuple(4, count * 2, buff));
				else if (type == "VEC4")
					data.emplace(std::piecewise_construct, std::forward_as_tuple(att), std::forward_as_tuple(4, count * 4, buff));

				hdr.num_vertices = count;
			}
		}

		{
			const auto& indices = primitives.find("indices");
			uint32_t accessor_index = indices.value();
			const auto& accessor = gltf["accessors"][accessor_index];
			const auto& bufferView = gltf["bufferViews"][accessor_index];
			const uint8_t* buff = buffer_data.data() + (uint32_t)bufferView["byteOffset"];
			const auto& componentType = accessor["componentType"];
			const uint32_t count = accessor["count"];
			if (componentType == GL_UNSIGNED_SHORT)
				data.emplace(std::piecewise_construct, std::forward_as_tuple("indices"), std::forward_as_tuple(2, count, buff));
			else if (componentType == GL_UNSIGNED_INT)
				data.emplace(std::piecewise_construct, std::forward_as_tuple("indices"), std::forward_as_tuple(4, count, buff));
		}

		assert(data.find("POSITION") != data.cend());

		const Vec3* vertices = (Vec3*)data.find("POSITION")->second.data;
		const Vec2* uvs = nullptr;
		if (data.find("TEXCOORD_0") != data.end())
			uvs = (Vec2*)data.find("TEXCOORD_0")->second.data;
		const Vec3* normals = nullptr;
		if (data.find("NORMAL") != data.end())
			normals = (Vec3*)data.find("NORMAL")->second.data;

		const auto& ind = data.find("indices")->second;
		hdr.num_indices = ind.count;
		uint32_t* indices = new uint32_t[hdr.num_indices];
		if (ind.stride == 2)
		{
			const uint16_t* const index_data = (const uint16_t*)ind.data;
			for (size_t i = 0; i < hdr.num_indices; i++)
				indices[i] = index_data[i];
		}
		else if (ind.stride == 4)
		{
			memcpy_s(indices, sizeof(uint32_t) * hdr.num_indices, ind.data, sizeof(uint32_t));
		}

		Vertex* vertex_data = new Vertex[hdr.num_vertices];
		Mat4* inverse_bind_matrices = nullptr;

		const auto& joint_ids_data = data.find("JOINTS_0");
		if (joint_ids_data != data.end())
		{
			hdr.type |= ASSET_TYPE::SKINNED_MODEL;

			assert(data.find("WEIGHTS_0") != data.end());
			const uint8_t* const joint = (const uint8_t*)joint_ids_data->second.data;
			std::vector<uVec4> joint_ids;
			joint_ids.reserve(hdr.num_vertices);
			for (uint32_t i = 0; i < hdr.num_vertices; i++)
				joint_ids.emplace_back(joint[i * 4], joint[i * 4 + 1], joint[i * 4 + 2], joint[i * 4 + 3]);

			const Vec4* const weights = (const Vec4*)data.find("WEIGHTS_0")->second.data;
			std::vector<Vec4> joint_weights(weights, weights + hdr.num_vertices);

			for (uint32_t i = 0; i < hdr.num_vertices; i++)
				new (&vertex_data[i]) Vertex(vertices[i], uvs[i], 0, normals[i], Vec3{ 0.0f, 0.0f, 0.0f }, Vec3{ 0.0f, 0.0f, 0.0f }, joint_ids[i], joint_weights[i]);

			assert(gltf.find("skins") != gltf.end());
			const uint32_t inv_bind_matrix_accessor_location = gltf["skins"][0]["inverseBindMatrices"];
			assert(gltf.find("accessors") != gltf.end());
			auto& inverse_bind_matrix_accessor = gltf["accessors"][inv_bind_matrix_accessor_location];
			const uint32_t& inv_bind_matrix_bufferView = inverse_bind_matrix_accessor["bufferView"];
			const uint32_t& inv_bind_matrix_count = inverse_bind_matrix_accessor["count"];
			assert(gltf.find("bufferViews") != gltf.end());
			auto& inverse_bind_matrix_bufferView = gltf["bufferViews"][inv_bind_matrix_bufferView];
			const uint32_t byteOffset = inverse_bind_matrix_bufferView["byteOffset"];

			inverse_bind_matrices = new Mat4[inv_bind_matrix_count];
			memcpy_s(inverse_bind_matrices, sizeof(Mat4) * inv_bind_matrix_count, &buffer_data[byteOffset], sizeof(Mat4) * inv_bind_matrix_count);

			hdr.num_joints = (uint32_t)gltf["skins"][0]["joints"].size();
		}
		else
		{
			for (uint32_t i = 0; i < hdr.num_vertices; i++)
				new (&vertex_data[i]) Vertex(vertices[i], uvs[i], 0, normals[i], Vec3{ 0.0f, 0.0f, 0.0f }, Vec3{ 0.0f, 0.0f, 0.0f }, uVec4{ 0, 0, 0, 0 }, Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		hdr.vertex_data = vertex_data;
		hdr.indices = indices;
		hdr.inverse_bind_matrices = inverse_bind_matrices;

		std::string new_file_name = std::string(file_name);
		new_file_name = new_file_name.substr(0, new_file_name.length() - 5);
		hdr.asset_name = new_file_name;
		new_file_name += ".glacier";
		WriteDataToFile(new_file_name, hdr);

		delete[] indices;
		delete[] vertex_data;
		delete[] inverse_bind_matrices;
	}

	void WriteDataToFile(const std::string& file_name, const GlacierFileHdr& data)
	{
		std::ofstream file(file_name, std::ofstream::binary);
		if (!file.is_open())
		{
			printf("Couldn't create new file!\n");
			return;
		}
		printf("Writing to file %s...\n", file_name.c_str());
		file.write(data.asset_name.c_str(), data.asset_name.length() + 1);
		file.write((const char*)&data.type, sizeof(uint32_t));
		file.write((const char*)&data.num_vertices, sizeof(uint32_t));
		file.write((const char*)&data.num_indices, sizeof(uint32_t));
		file.write((const char*)&data.num_textures, sizeof(uint32_t));
		file.write((const char*)&data.num_joints, sizeof(uint32_t));
		file.write((const char*)data.vertex_data, sizeof(Vertex) * data.num_vertices);
		file.write((const char*)data.indices, sizeof(uint32_t) * data.num_indices);
		if(data.num_joints > 0)
			file.write((const char*)data.inverse_bind_matrices, sizeof(Mat4) * data.num_joints);

		printf("Done.\n");
	}
}