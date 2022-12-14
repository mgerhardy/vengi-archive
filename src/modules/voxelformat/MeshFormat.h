/**
 * @file
 */

#pragma once

#include "Format.h"
#include "private/Tri.h"
#include "core/collection/DynamicArray.h"
#include "core/collection/Map.h"

namespace voxelformat {

/**
 * @brief Convert the volume data into a mesh
 *
 * http://research.michael-schwarz.com/publ/2010/vox/
 * http://research.michael-schwarz.com/publ/files/vox-siga10.pdf
 * https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.12.6294
 */
class MeshFormat : public Format {
public:
	static constexpr const uint8_t FillColorIndex = 2;
	using TriCollection = core::DynamicArray<Tri, 512>;

	/**
	 * Subdivide until we brought the triangles down to the size of 1 or smaller
	 */
	static void subdivideTri(const Tri &tri, TriCollection &tinyTris);
	static bool calculateAABB(const TriCollection &tris, glm::vec3 &mins, glm::vec3 &maxs);
	static bool isAxisAligned(const TriCollection &tris);
protected:
	struct MeshExt {
		MeshExt(voxel::Mesh *mesh, const SceneGraphNode &node, bool applyTransform);
		voxel::Mesh *mesh;
		core::String name;
		bool applyTransform = false;

		glm::vec3 size{0.0f};
		int nodeId = -1;
	};
	using Meshes = core::DynamicArray<MeshExt>;
	virtual bool saveMeshes(const core::Map<int, int> &meshIdxNodeMap, const SceneGraph &sceneGraph,
							const Meshes &meshes, const core::String &filename, io::SeekableWriteStream &stream,
							const glm::vec3 &scale = glm::vec3(1.0f), bool quad = false, bool withColor = true,
							bool withTexCoords = true) = 0;

	static MeshExt* getParent(const voxelformat::SceneGraph &sceneGraph, Meshes &meshes, int nodeId);
	static glm::vec3 getScale();

	/**
	 * @brief Voxelizes the input mesh
	 *
	 * Convert your input mesh into @c Tri instances and use the methods of this class to help voxelizing those.
	 * @see voxelizeNode()
	 */
	virtual bool voxelizeGroups(const core::String &filename, io::SeekableReadStream& file, SceneGraph& sceneGraph);

	/**
	 * @see voxelizeGroups()
	 */
	bool voxelizeNode(const core::String &name, SceneGraph &sceneGraph, const TriCollection &tris);

	struct PosSamplingEntry {
		inline PosSamplingEntry(float _area, const glm::vec4 &_color) : area(_area), color(_color) {
		}
		float area;
		glm::vec4 color;
	};

	struct PosSampling {
		core::DynamicArray<PosSamplingEntry> entries;
		inline PosSampling(float area, const glm::vec4 &color) {
			entries.emplace_back(area, color);
		}
		glm::vec4 avgColor() const {
			if (entries.size() == 1) {
				return entries[0].color;
			}
			float sumArea = 0.0f;
			for (const PosSamplingEntry& pe : entries) {
				sumArea += pe.area;
			}
			glm::vec4 color{0.0f};
			if (sumArea <= 0.0f) {
				color[3] = 1.0f;
				return color;
			}
			for (const PosSamplingEntry& pe : entries) {
				color += pe.color * pe.area / sumArea;
			}
			color[3] = 1.0f;
			return color;
		}
	};

	typedef core::Map<glm::ivec3, PosSampling, 64, glm::hash<glm::ivec3>> PosMap;

	static void voxelizeTris(voxelformat::SceneGraphNode &node, const PosMap &posMap, bool hillHollow);
	static void transformTris(const TriCollection &subdivided, PosMap &posMap);
	static void transformTrisAxisAligned(const TriCollection &tris, PosMap &posMap);

public:
	bool loadGroups(const core::String &filename, io::SeekableReadStream &file, SceneGraph &sceneGraph) override;
	bool saveGroups(const SceneGraph &sceneGraph, const core::String &filename,
					io::SeekableWriteStream &stream, ThumbnailCreator thumbnailCreator) override;
};

} // namespace voxel
