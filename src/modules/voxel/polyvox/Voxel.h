/**
 * @file
 */

#pragma once

#include <stdint.h>
#include <SDL.h>
#include "core/Array.h"

namespace voxel {

/**
 * @brief material types 0 - 255 (8 bits)
 */
enum class VoxelType : uint8_t {
	// this must be 0
	Air = 0,
	Water,
	Generic,
	Grass,
	Wood,
	Leaf,
	LeafFir,
	LeafPine,
	Flower,
	Bloom,
	Mushroom,
	Rock,
	Sand,
	Cloud,
	Dirt,
	Roof,
	Wall,

	Max
};

static const char* VoxelTypeStr[] = {
	"Air",
	"Water",
	"Generic",
	"Grass",
	"Wood",
	"Leaf",
	"LeafFir",
	"LeafPine",
	"Flower",
	"Bloom",
	"Mushroom",
	"Rock",
	"Sand",
	"Cloud",
	"Dirt",
	"Roof",
	"Wall"
};
static_assert(lengthof(VoxelTypeStr) == (int)VoxelType::Max, "voxel type string array size doesn't match the available voxel types");

/**
 * @return VoxelType::Max if invalid string was given
 */
extern VoxelType getVoxelType(const char *str);

class Voxel {
public:
	constexpr Voxel() :
		_material(VoxelType::Air), _colorIndex(0) {
	}

	constexpr Voxel(VoxelType material, uint8_t colorIndex) :
		_material(material), _colorIndex(colorIndex) {
	}

	constexpr Voxel(const Voxel& voxel) :
		_material(voxel._material), _colorIndex(voxel._colorIndex) {
	}

	/**
	 * @brief Compares by the material type
	 */
	inline bool operator==(const Voxel& rhs) const {
		return _material == rhs._material;
	}

	/**
	 * @brief Compares by the material type
	 */
	inline bool operator!=(const Voxel& rhs) const {
		return !(*this == rhs);
	}

	inline bool isSame(const Voxel& other) const {
		return _material == other._material && _colorIndex == other._colorIndex;
	}

	/**
	 * @brief Compares by the material type
	 */
	inline bool isSameType(const Voxel& other) const {
		return _material == other._material;
	}

	inline uint8_t getColor() const {
		return _colorIndex;
	}

	inline void setColor(uint8_t colorIndex) {
		_colorIndex = colorIndex;
	}

	inline VoxelType getMaterial() const {
		return _material;
	}

	inline void setMaterial(VoxelType material) {
		_material = material;
	}

private:
	VoxelType _material;
	uint8_t _colorIndex;
};

constexpr Voxel createVoxel(VoxelType type, uint8_t colorIndex) {
	return Voxel(type, colorIndex);
}

inline bool isBlocked(VoxelType material) {
	return material != VoxelType::Air;
}

inline bool isWater(VoxelType material) {
	return material == VoxelType::Water;
}

inline bool isLeaves(VoxelType material) {
	return material == VoxelType::Leaf;
}

SDL_FORCE_INLINE bool isAir(VoxelType material) {
	return material == VoxelType::Air;
}

inline bool isWood(VoxelType material) {
	return material == VoxelType::Wood;
}

inline bool isGrass(VoxelType material) {
	return material == VoxelType::Grass;
}

inline bool isRock(VoxelType material) {
	return material == VoxelType::Rock;
}

inline bool isSand(VoxelType material) {
	return material == VoxelType::Sand;
}

inline bool isDirt(VoxelType material) {
	return material == VoxelType::Dirt;
}

inline bool isFloor(VoxelType material) {
	return isRock(material) || isDirt(material) || isSand(material) || isGrass(material);
}

}
