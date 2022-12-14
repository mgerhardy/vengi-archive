/**
 * @file
 */

#include "VolumeRotator.h"
#include "math/Axis.h"
#include "voxel/RawVolume.h"
#include "math/AABB.h"
#include "core/GLM.h"
#include "core/Assert.h"
#include "voxel/Region.h"
#include "voxel/Voxel.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace voxelutil {

static inline glm::vec4 transform(const glm::mat4x4 &mat, const glm::ivec3 &pos, const glm::vec4 &pivot) {
	return glm::floor(mat * (glm::vec4((float)pos.x + 0.5f, (float)pos.y + 0.5f, (float)pos.z + 0.5f, 1.0f) - pivot));
}

/**
 * @param[in] source The RawVolume to rotate
 * @param[in] angles The angles for the x, y and z axis given in degrees
 * @return A new RawVolume. It's the caller's responsibility to free this
 * memory.
 */
voxel::RawVolume* rotateVolume(const voxel::RawVolume* source, const glm::vec3& angles, const glm::vec3& pivot) {
	const float pitch = glm::radians(angles.x);
	const float yaw = glm::radians(angles.y);
	const float roll = glm::radians(angles.z);
	const glm::mat4& mat = glm::eulerAngleXYZ(pitch, yaw, roll);
	const voxel::Region& srcRegion = source->region();
	const glm::ivec3 maxs = srcRegion.getDimensionsInCells();

	const glm::ivec3& transformedMins = transform(mat, glm::ivec3(0), glm::vec4(pivot, 0.0f));
	const glm::ivec3& transformedMaxs = transform(mat, maxs, glm::vec4(pivot, 0.0f));
	const voxel::Region region(glm::min(transformedMins, transformedMaxs), glm::max(transformedMins, transformedMaxs));
	voxel::RawVolume* destination = new voxel::RawVolume(region);
	voxel::RawVolume::Sampler destSampler(destination);
	voxel::RawVolume::Sampler srcSampler(source);

	for (int32_t z = srcRegion.getLowerZ(); z <= srcRegion.getUpperZ(); ++z) {
		for (int32_t y = srcRegion.getLowerY(); y <= srcRegion.getUpperY(); ++y) {
			srcSampler.setPosition(srcRegion.getLowerX(), y, z);
			for (int32_t x = srcRegion.getLowerX(); x <= srcRegion.getUpperX(); ++x) {
				const voxel::Voxel &voxel = srcSampler.voxel();
				if (!voxel::isAir(voxel.getMaterial())) {
					glm::ivec3 destPos = glm::ivec3(x - srcRegion.getLowerX(), y - srcRegion.getLowerY(), z - srcRegion.getLowerZ());
					const glm::ivec3 &volumePos = transform(mat, destPos, glm::vec4(pivot, 0.0f));
					destSampler.setPosition(volumePos);
					core_assert_msg(region.containsPoint(volumePos), "volumepos %i:%i:%i is not part of the rotated region %s", volumePos.x, volumePos.y, volumePos.z, region.toString().c_str());
					destSampler.setVoxel(voxel);
				}
				srcSampler.movePositiveX();
			}
		}
	}
	const glm::ivec3 &delta = srcRegion.getCenter() - destination->region().getCenter();
	destination->translate(delta);
	return destination;
}

voxel::RawVolume* rotateAxis(const voxel::RawVolume* source, math::Axis axis) {
	const voxel::Region& region = source->region();
	glm::vec3 rotVec{0.0f};
	rotVec[math::getIndexForAxis(axis)] = 90.0f;
	return rotateVolume(source, rotVec, region.getPivot());
}

voxel::RawVolume* mirrorAxis(const voxel::RawVolume* source, math::Axis axis) {
	const voxel::Region& srcRegion = source->region();
	voxel::RawVolume* destination = new voxel::RawVolume(source);
	voxel::RawVolume::Sampler destSampler(destination);
	voxel::RawVolume::Sampler srcSampler(source);

	const glm::ivec3& mins = srcRegion.getLowerCorner();
	const glm::ivec3& maxs = srcRegion.getUpperCorner();

	if (axis == math::Axis::X) {
		for (int32_t z = mins.z; z <= maxs.z; ++z) {
			for (int32_t y = mins.y; y <= maxs.y; ++y) {
				srcSampler.setPosition(mins.x, y, z);
				destSampler.setPosition(maxs.x, y, z);
				for (int32_t x = mins.x; x <= maxs.x; ++x) {
					destSampler.setVoxel(srcSampler.voxel());
					srcSampler.movePositiveX();
					destSampler.moveNegativeX();
				}
			}
		}
	} else if (axis == math::Axis::Y) {
		for (int32_t z = mins.z; z <= maxs.z; ++z) {
			for (int32_t x = mins.x; x <= maxs.x; ++x) {
				srcSampler.setPosition(x, mins.y, z);
				destSampler.setPosition(x, maxs.y, z);
				for (int32_t y = mins.y; y <= maxs.y; ++y) {
					destSampler.setVoxel(srcSampler.voxel());
					srcSampler.movePositiveY();
					destSampler.moveNegativeY();
				}
			}
		}
	} else if (axis == math::Axis::Z) {
		for (int32_t y = mins.y; y <= maxs.y; ++y) {
			for (int32_t x = mins.x; x <= maxs.x; ++x) {
				srcSampler.setPosition(x, y, mins.z);
				destSampler.setPosition(x, y, maxs.z);
				for (int32_t z = mins.z; z <= maxs.z; ++z) {
					destSampler.setVoxel(srcSampler.voxel());
					srcSampler.movePositiveZ();
					destSampler.moveNegativeZ();
				}
			}
		}
	}
	return destination;
}

}
