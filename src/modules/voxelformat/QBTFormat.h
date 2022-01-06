/**
 * @file
 */

#pragma once

#include "Format.h"

namespace voxel {

/**
 * @brief Qubicle Binary Tree (qbt) is the successor of the widespread voxel exchange format Qubicle Binary.
 *
 * @see QBFormat
 *
 * https://getqubicle.com/qubicle/documentation/docs/file/qbt/
 */
class QBTFormat : public Format {
private:
	bool skipNode(io::SeekableReadStream& stream);
	bool loadMatrix(io::SeekableReadStream& stream, SceneGraph &sceneGraph);
	bool loadCompound(io::SeekableReadStream& stream, SceneGraph &sceneGraph);
	bool loadModel(io::SeekableReadStream& stream, SceneGraph &sceneGraph);
	bool loadNode(io::SeekableReadStream& stream, SceneGraph &sceneGraph);

	bool loadColorMap(io::SeekableReadStream& stream);
	bool loadFromStream(io::SeekableReadStream& stream, SceneGraph &sceneGraph);
	bool saveMatrix(io::SeekableWriteStream& stream, const SceneGraphNode& volume, bool colorMap) const;
	bool saveColorMap(io::SeekableWriteStream& stream) const;
	bool saveModel(io::SeekableWriteStream& stream, const SceneGraph &sceneGraph, bool colorMap) const;
public:
	bool loadGroups(const core::String &filename, io::SeekableReadStream& stream, SceneGraph &sceneGraph) override;
	bool saveGroups(const SceneGraph &sceneGraph, const core::String &filename, io::SeekableWriteStream& stream) override;
};

}
