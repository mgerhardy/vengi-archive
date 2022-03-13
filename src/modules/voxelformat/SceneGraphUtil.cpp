/**
 * @file
 */

#include "SceneGraphUtil.h"
#include "core/Log.h"
#include "voxel/RawVolume.h"

namespace voxel {

static int addToGraph(voxel::SceneGraph &sceneGraph, voxel::SceneGraphNode &&node, int parent) {
	int newNodeId = sceneGraph.emplace(core::move(node), parent);
	if (newNodeId == -1) {
		Log::error("Failed to add node to the scene");
		return -1;
	}
	return newNodeId;
}

static void copy(const voxel::SceneGraphNode &node, voxel::SceneGraphNode &target) {
	target.setName(node.name());
	target.setKeyFrames(node.keyFrames());
	target.setVisible(node.visible());
	target.addProperties(node.properties());
	if (node.type() == voxel::SceneGraphNodeType::Model) {
		core_assert(node.volume() != nullptr);
	} else {
		core_assert(node.volume() == nullptr);
	}
}

int addNodeToSceneGraph(voxel::SceneGraph &sceneGraph, const voxel::SceneGraphNode &node, int parent) {
	voxel::SceneGraphNode newNode(node.type());
	copy(node, newNode);
	if (newNode.type() == voxel::SceneGraphNodeType::Model) {
		newNode.setVolume(new voxel::RawVolume(node.volume()), true);
	}
	return addToGraph(sceneGraph, core::move(newNode), parent);
}

int addNodeToSceneGraph(voxel::SceneGraph &sceneGraph, voxel::SceneGraphNode &node, int parent) {
	voxel::SceneGraphNode newNode(node.type());
	copy(node, newNode);
	if (newNode.type() == voxel::SceneGraphNodeType::Model) {
		core_assert(node.owns());
		newNode.setVolume(node.volume(), true);
		node.releaseOwnership();
	}
	return addToGraph(sceneGraph, core::move(newNode), parent);
}

static int addSceneGraphNode_r(voxel::SceneGraph& sceneGraph, voxel::SceneGraph &newSceneGraph, voxel::SceneGraphNode &node, int parent) {
	const int newNodeId = addNodeToSceneGraph(sceneGraph, node, parent);
	if (newNodeId == -1) {
		Log::error("Failed to add node to the scene graph");
		return 0;
	}

	const voxel::SceneGraphNode &newNode = newSceneGraph.node(newNodeId);
	int nodesAdded = newNode.type() == voxel::SceneGraphNodeType::Model ? 1 : 0;
	for (int nodeIdx : newNode.children()) {
		core_assert(newSceneGraph.hasNode(nodeIdx));
		voxel::SceneGraphNode &childNode = newSceneGraph.node(nodeIdx);
		nodesAdded += addSceneGraphNode_r(sceneGraph, newSceneGraph, childNode, newNodeId);
	}

	return nodesAdded;
}

int addSceneGraphNodes(voxel::SceneGraph& sceneGraph, voxel::SceneGraph& newSceneGraph, int parent) {
	const voxel::SceneGraphNode &root = newSceneGraph.root();
	int nodesAdded = 0;
	sceneGraph.node(parent).addProperties(root.properties());
	for (int nodeId : root.children()) {
		nodesAdded += addSceneGraphNode_r(sceneGraph, newSceneGraph, newSceneGraph.node(nodeId), parent);
	}
	return nodesAdded;
}

} // namespace voxel