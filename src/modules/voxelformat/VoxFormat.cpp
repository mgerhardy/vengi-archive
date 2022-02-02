/**
 * @file
 */

#include "VoxFormat.h"
#include "core/Common.h"
#include "core/FourCC.h"
#include "core/Color.h"
#include "core/ArrayLength.h"
#include "core/Assert.h"
#include "core/Log.h"
#include "core/StandardLib.h"
#include "core/StringUtil.h"
#include "core/collection/DynamicArray.h"
#include "math/Math.h"
#include "voxel/MaterialColor.h"
#include "voxel/RawVolume.h"
#include "voxel/Voxel.h"
#include "voxelformat/SceneGraph.h"
#include "voxelformat/SceneGraphNode.h"
#include "voxelutil/VolumeVisitor.h"

#define OGT_VOX_IMPLEMENTATION
#include "external/ogt_vox.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

namespace voxel {

static void* _ogt_alloc(size_t size) {
	return core_malloc(size);
}

static void _ogt_free(void *mem) {
	core_free(mem);
}

static const ogt_vox_transform ogt_identity_transform {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

VoxFormat::VoxFormat() {
	ogt_vox_set_memory_allocator(_ogt_alloc, _ogt_free);
}

size_t VoxFormat::loadPalette(const core::String &filename, io::SeekableReadStream &stream, core::Array<uint32_t, 256> &palette) {
	const size_t size = stream.size();
	uint8_t *buffer = (uint8_t *)core_malloc(size);
	if (stream.read(buffer, size) == -1) {
		core_free(buffer);
		return 0;
	}
	const ogt_vox_scene *scene = ogt_vox_read_scene_with_flags(buffer, size, 0);
	core_free(buffer);
	if (scene == nullptr) {
		Log::error("Could not load scene %s", filename.c_str());
		return 0;
	}
	for (size_t i = 0; i < palette.size(); ++i) {
		const ogt_vox_rgba& c = scene->palette.color[i];
		_colors[i] = palette[i] = core::Color::getRGBA(c.r, c.g, c.b, c.a);
		_palette[i] = i;
	}
	_colorsSize = 256;
	_paletteSize = 256;
	ogt_vox_destroy_scene(scene);
	return palette.size();
}

/**
 * @brief Calculate the scene graph object transformation. Used for the voxel and the AABB of the volume.
 *
 * @param mat The world space model matrix (rotation and translation) for the chunk
 * @param pos The position inside the untransformed chunk (local position)
 * @param pivot The pivot to do the rotation around. This is the @code chunk_size - 1 + 0.5 @endcode. Please
 * note that the @c w component must be @c 0.0
 * @return glm::vec4 The transformed world position
 */
static inline glm::vec4 transform(const glm::mat4x4 &mat, const glm::ivec3 &pos, const glm::vec4 &pivot) {
	return glm::floor(mat * (glm::vec4((float)pos.x + 0.5f, (float)pos.y + 0.5f, (float)pos.z + 0.5f, 1.0f) - pivot));
}

bool VoxFormat::addInstance(const ogt_vox_scene *scene, uint32_t ogt_instanceIdx, SceneGraph &sceneGraph, int parent, const glm::mat4 &zUpMat, bool groupHidden) {
	const ogt_vox_instance& ogtInstance = scene->instances[ogt_instanceIdx];
	const ogt_vox_transform& ogtTransform = ogtInstance.transform;
	const glm::vec4 ogtCol0(ogtTransform.m00, ogtTransform.m01, ogtTransform.m02, ogtTransform.m03);
	const glm::vec4 ogtCol1(ogtTransform.m10, ogtTransform.m11, ogtTransform.m12, ogtTransform.m13);
	const glm::vec4 ogtCol2(ogtTransform.m20, ogtTransform.m21, ogtTransform.m22, ogtTransform.m23);
	const glm::vec4 ogtCol3(ogtTransform.m30, ogtTransform.m31, ogtTransform.m32, ogtTransform.m33);
	const glm::mat4 ogtMat = glm::mat4{ogtCol0, ogtCol1, ogtCol2, ogtCol3};
	const ogt_vox_model *ogtModel = scene->models[ogtInstance.model_index];
	const uint8_t *ogtVoxels = ogtModel->voxel_data;
	const uint8_t *ogtVoxel = ogtVoxels;
	const glm::ivec3 maxs(ogtModel->size_x - 1, ogtModel->size_y - 1, ogtModel->size_z - 1);
	const glm::vec4 pivot(glm::floor((float)ogtModel->size_x / 2.0f), glm::floor((float)ogtModel->size_y / 2.0f), glm::floor((float)ogtModel->size_z / 2.0f), 0.0f);
	const glm::ivec3& transformedMins = transform(ogtMat, glm::ivec3(0), pivot);
	const glm::ivec3& transformedMaxs = transform(ogtMat, maxs, pivot);
	const glm::ivec3& zUpMins = transform(zUpMat, transformedMins, glm::ivec4(0));
	const glm::ivec3& zUpMaxs = transform(zUpMat, transformedMaxs, glm::ivec4(0));
	const voxel::Region region(glm::min(zUpMins, zUpMaxs), glm::max(zUpMins, zUpMaxs));
	voxel::RawVolume *v = new voxel::RawVolume(region);

	for (uint32_t k = 0; k < ogtModel->size_z; ++k) {
		for (uint32_t j = 0; j < ogtModel->size_y; ++j) {
			for (uint32_t i = 0; i < ogtModel->size_x; ++i, ++ogtVoxel) {
				if (ogtVoxel[0] == 0) {
					continue;
				}
				const voxel::Voxel voxel = voxel::createVoxel(voxel::VoxelType::Generic, _palette[ogtVoxel[0]]);
				const glm::ivec3& pos = transform(ogtMat, glm::ivec3(i, j, k), pivot);
				const glm::ivec3& poszUp = transform(zUpMat, pos, glm::ivec4(0));
				v->setVoxel(poszUp, voxel);
			}
		}
	}

	SceneGraphNode node(SceneGraphNodeType::Model);
	const char *name = ogtInstance.name;
	if (name == nullptr) {
		const ogt_vox_layer& layer = scene->layers[ogtInstance.layer_index];
		name = layer.name;
		if (name == nullptr) {
			name = "";
		}
	}
	node.setName(name);
	node.setVisible(!ogtInstance.hidden && !groupHidden);
	node.setVolume(v, true);
	return sceneGraph.emplace(core::move(node), parent) != -1;
}

bool VoxFormat::addGroup(const ogt_vox_scene *scene, uint32_t ogt_parentGroupIdx, SceneGraph &sceneGraph, int parent, const glm::mat4 &zUpMat, core::Set<uint32_t> &addedInstances) {
	const ogt_vox_group &group = scene->groups[ogt_parentGroupIdx];
	bool hidden = group.hidden;
	const char *name = "Group";
	const uint32_t layerIdx = group.layer_index;
	if (layerIdx < scene->num_layers) {
		const ogt_vox_layer &layer = scene->layers[layerIdx];
		hidden |= layer.hidden;
		if (layer.name != nullptr) {
			name = layer.name;
		}
	}
	SceneGraphNode node(SceneGraphNodeType::Group);
	node.setName(name);
	node.setVisible(!hidden);
	const int groupId = sceneGraph.emplace(core::move(node), parent);
	if (groupId == -1) {
		Log::error("Failed to add group node to the scene graph");
		return false;
	}

	for (uint32_t n = 0; n < scene->num_instances; ++n) {
		const ogt_vox_instance& ogtInstance = scene->instances[n];
		if (ogtInstance.group_index != ogt_parentGroupIdx) {
			continue;
		}
		if (!addedInstances.insert(n)) {
			continue;
		}
		if (!addInstance(scene, n, sceneGraph, groupId, zUpMat, hidden)) {
			return false;
		}
	}

	for (uint32_t groupIdx = 0; groupIdx < scene->num_groups; ++groupIdx) {
		const ogt_vox_group &group = scene->groups[groupIdx];
		Log::debug("group %u with parent: %u (searching for %u)", groupIdx, group.parent_group_index, ogt_parentGroupIdx);
		if (group.parent_group_index != ogt_parentGroupIdx) {
			continue;
		}
		Log::debug("Found matching group (%u) with scene graph parent: %i", groupIdx, groupId);
		if (!addGroup(scene, groupIdx, sceneGraph, groupId, zUpMat, addedInstances)) {
			return false;
		}
	}

	return groupId;
}

bool VoxFormat::loadGroups(const core::String &filename, io::SeekableReadStream &stream, SceneGraph &sceneGraph) {
	const size_t size = stream.size();
	uint8_t *buffer = (uint8_t *)core_malloc(size);
	if (stream.read(buffer, size) == -1) {
		core_free(buffer);
		return false;
	}
	const ogt_vox_scene *scene = ogt_vox_read_scene_with_flags(buffer, size, k_read_scene_flags_groups);
	core_free(buffer);
	if (scene == nullptr) {
		Log::error("Could not load scene %s", filename.c_str());
		return false;
	}

	for (int i = 0; i < (int)_palette.size(); ++i) {
		const ogt_vox_rgba color = scene->palette.color[i];
		const glm::vec4& colorVec = core::Color::fromRGBA(color.r, color.g, color.b, color.a);
		_colors[i] = core::Color::getRGBA(colorVec);
		const uint8_t index = findClosestIndex(colorVec);
		_palette[i] = index;
	}
	_colorsSize = _colors.size();
	// rotation matrix to convert into our coordinate system (z pointing upwards)
	const glm::mat4 zUpMat = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	core::Set<uint32_t> addedInstances;

	if (scene->num_groups > 0) {
		for (uint32_t i = 0; i < scene->num_groups; ++i) {
			const ogt_vox_group &group = scene->groups[i];
			// find the main group nodes
			if (group.parent_group_index != k_invalid_group_index) {
				continue;
			}
			Log::debug("Add root group %u/%u", i, scene->num_groups);
			if (!addGroup(scene, i, sceneGraph, sceneGraph.root().id(), zUpMat, addedInstances)) {
				ogt_vox_destroy_scene(scene);
				return false;
			}
		}
	}
	for (uint32_t n = 0; n < scene->num_instances; ++n) {
		if (addedInstances.has(n)) {
			continue;
		}
		if (!addInstance(scene, n, sceneGraph, sceneGraph.root().id(), zUpMat)) {
			ogt_vox_destroy_scene(scene);
			return false;
		}
	}

	ogt_vox_destroy_scene(scene);
	return true;
}

int VoxFormat::findClosestPaletteIndex() {
	// we have to find a replacement for the first palette entry - as this is used
	// as the empty voxel in magicavoxel
	voxel::MaterialColorArray materialColors = voxel::getMaterialColors();
	const glm::vec4 first = materialColors[0];
	materialColors.erase(materialColors.begin());
	return core::Color::getClosestMatch(first, materialColors) + 1;
}

bool VoxFormat::saveGroups(const SceneGraph &sceneGraph, const core::String &filename, io::SeekableWriteStream &stream) {
	SceneGraph newSceneGraph;
	splitVolumes(sceneGraph, newSceneGraph, glm::ivec3(126));

	ogt_vox_group default_group;
	default_group.hidden = false;
	default_group.layer_index = 0;
	default_group.parent_group_index = k_invalid_group_index;
	default_group.transform = ogt_identity_transform;

	core::DynamicArray<ogt_vox_model> models;
	core::DynamicArray<ogt_vox_layer> layers;
	core::DynamicArray<const ogt_vox_model*> modelPtrs;
	core::DynamicArray<ogt_vox_instance> instances;

	const int replacement = findClosestPaletteIndex();
	const glm::vec4 emptyColor = getColor(voxel::Voxel(voxel::VoxelType::Generic, 0));
	const glm::vec4 replaceColor = getColor(voxel::Voxel(voxel::VoxelType::Generic, replacement));
	Log::debug("Replacement for %f:%f:%f:%f is at %i (%f:%f:%f:%f)", emptyColor.r, emptyColor.g, emptyColor.b,
			   emptyColor.a, replacement, replaceColor.r, replaceColor.g, replaceColor.b, replaceColor.a);

	for (const SceneGraphNode &node : newSceneGraph) {
		const voxel::Region region = node.region();
		ogt_vox_model model;
		// flip y and z here
		model.size_x = region.getWidthInVoxels();
		model.size_y = region.getDepthInVoxels();
		model.size_z = region.getHeightInVoxels();
		const int voxelSize = (int)(model.size_x * model.size_y * model.size_z);
		uint8_t *dataptr = (uint8_t*)core_malloc(voxelSize);
		model.voxel_data = dataptr;
		voxelutil::visitVolume(*node.volume(), [&] (int, int, int, const voxel::Voxel& voxel) {
			if (voxel.getColor() == 0 && !isAir(voxel.getMaterial())) {
				*dataptr++ = replacement;
			} else {
				*dataptr++ = voxel.getColor();
			}
		}, voxelutil::VisitAll(), voxelutil::VisitorOrder::YZmX);
		models.push_back(model);
		modelPtrs.push_back(&models[models.size() - 1]);

		ogt_vox_layer layer;
		layer.name = node.name().c_str();
		layer.hidden = !node.visible();
		layers.push_back(layer);

		ogt_vox_instance instance;
		instance.group_index = 0;
		instance.model_index = models.size() - 1;
		instance.layer_index = layers.size() - 1;
		instance.name = node.name().c_str();
		instance.hidden = !node.visible();
		const glm::vec3 &mins = region.getLowerCornerf();
		const glm::vec3 &maxs = region.getUpperCornerf();
		const glm::vec3 width = maxs - mins + 1.0f;
		const glm::vec3 transform = mins + width / 2.0f;
		// y and z are flipped here
		instance.transform = ogt_identity_transform;
		instance.transform.m30 = -glm::floor(transform.x + 0.5f);
		instance.transform.m31 = transform.z;
		instance.transform.m32 = transform.y;
		instances.push_back(instance);
	}

	ogt_vox_scene output_scene;
	output_scene.groups = &default_group;
	output_scene.num_groups = 1;
	output_scene.instances = &instances[0];
	output_scene.num_instances = instances.size();
	output_scene.layers = &layers[0];
	output_scene.num_layers = layers.size();
	output_scene.models = &modelPtrs[0];
	output_scene.num_models = models.size();
	core_memset(&output_scene.materials, 0, sizeof(output_scene.materials));

	ogt_vox_palette& pal = output_scene.palette;
	const MaterialColorArray& materialColors = getMaterialColors();
	for (int i = 0; i < 256; ++i) {
		pal.color[i].r = (uint8_t)(materialColors[i].r * 255.0f);
		pal.color[i].g = (uint8_t)(materialColors[i].g * 255.0f);
		pal.color[i].b = (uint8_t)(materialColors[i].b * 255.0f);
		pal.color[i].a = (uint8_t)(materialColors[i].a * 255.0f);
	}

	uint32_t buffersize = 0;
	uint8_t *buffer = ogt_vox_write_scene(&output_scene, &buffersize);
	if (!buffer) {
		Log::error("Failed to write the scene");
		return false;
	}
	if (stream.write(buffer, buffersize) == -1) {
		Log::error("Failed to write to the stream");
		return false;
	}
	ogt_vox_free(buffer);

	for (ogt_vox_model & m : models) {
		core_free((void*)m.voxel_data);
	}

	return true;
}

} // namespace voxel
