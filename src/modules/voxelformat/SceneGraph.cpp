/**
 * @file
 */

#include "SceneGraph.h"
#include "core/Common.h"
#include "core/Log.h"
#include "voxel/RawVolume.h"
#include "voxelformat/SceneGraphNode.h"
#include "voxelutil/VolumeMerger.h"

namespace voxel {

SceneGraph::SceneGraph() {
	clear();
}

SceneGraph::~SceneGraph() {
	_nodes.clear();
}

const SceneGraphNode& SceneGraph::root() const {
	auto iter = _nodes.find(0);
	core_assert(iter != _nodes.end());
	return iter->value;
}

void SceneGraph::emplace_back(SceneGraphNode &&node, int parent) {
	const int nodeId = _nextNodeId++;
	node.setId(nodeId);
	_nodes.emplace(nodeId, core::forward<SceneGraphNode>(node));
	if (parent >= 0) {
		auto iter = _nodes.find(parent);
		if (iter == _nodes.end()) {
			Log::error("Could not find parent node with id %i", parent);
			return;
		}
		iter->value.addChild(nodeId);
	}
}

void SceneGraph::reserve(size_t size) {
}

bool SceneGraph::empty() const {
	return begin() == end();
}

size_t SceneGraph::size(SceneGraphNodeType type) const {
	auto begin = iterator(_nodes.begin(), _nodes.end(), type);
	auto end = iterator(_nodes.end(), _nodes.end(), type);
	size_t n = 0;
	for (auto iter = begin; iter != end; ++iter) {
		++n;
	}
	return n;
}

void SceneGraph::clear() {
	for (const auto &entry : _nodes) {
		entry->value.release();
	}
	_nodes.clear();
	_nextNodeId = 0;
	SceneGraphNode node(SceneGraphNodeType::Root);
	node.setName("root");
	emplace_back(core::move(node), -1);
}

const SceneGraphNode &SceneGraph::operator[](int modelIdx) const {
	iterator iter = iterator(_nodes.begin(), _nodes.end(), SceneGraphNodeType::Model);
	for (int i = 0; i < modelIdx; ++i) {
		++iter;
	}
	return *iter;
}

SceneGraphNode &SceneGraph::operator[](int modelIdx) {
	iterator iter = iterator(_nodes.begin(), _nodes.end(), SceneGraphNodeType::Model);
	for (int i = 0; i < modelIdx; ++i) {
		++iter;
	}
	return *iter;
}

voxel::RawVolume *SceneGraph::merge() const {
	core::DynamicArray<const voxel::RawVolume *> rawVolumes;
	rawVolumes.reserve(_nodes.size() - 1);
	for (const auto &node : *this) {
		core_assert(node.type() == SceneGraphNodeType::Model);
		core_assert(node.volume() != nullptr);
		rawVolumes.push_back(node.volume());
	}
	if (rawVolumes.empty()) {
		return nullptr;
	}
	if (rawVolumes.size() == 1) {
		return new voxel::RawVolume(rawVolumes[0]);
	}
	return ::voxel::merge(rawVolumes);
}

} // namespace voxel
