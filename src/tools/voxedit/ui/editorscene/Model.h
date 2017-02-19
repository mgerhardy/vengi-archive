#pragma once

#include "voxel/polyvox/Picking.h"
#include "voxel/polyvox/RawVolume.h"
#include "voxel/generator/LSystemGenerator.h"
#include "voxel/generator/PlantGenerator.h"
#include "voxel/generator/TreeGenerator.h"
#include "voxel/generator/BuildingGeneratorContext.h"
#include "frontend/RawVolumeRenderer.h"
#include "video/ShapeBuilder.h"
#include "frontend/ShapeRenderer.h"
#include "Action.h"
#include "voxedit-util/SelectionHandler.h"
#include "voxedit-util/ShapeHandler.h"
#include "voxedit-util/UndoHandler.h"
#include "voxedit-util/Axis.h"
#include "voxel/WorldContext.h"
#include <vector>

namespace voxedit {

static constexpr int ModelVolumeIndex = 0;
static constexpr int CursorVolumeIndex = 1;
static constexpr int SelectionVolumeIndex = 0;

/**
 * The model is shared across all viewports
 */
class Model {
private:
	frontend::RawVolumeRenderer _rawVolumeRenderer;
	frontend::RawVolumeRenderer _rawVolumeSelectionRenderer;
	video::ShapeBuilder _shapeBuilder;
	frontend::ShapeRenderer _shapeRenderer;
	UndoHandler _undoHandler;
	SelectionHandler _selectionHandler;
	ShapeHandler _shapeHandler;

	int32_t _planeMeshIndex[3] = {-1, -1, -1};

	int _initialized = 0;
	int _size = 32;
	int _mouseX = 0;
	int _mouseY = 0;

	glm::ivec3 _lastPlacement;
	glm::ivec3 _cursorPos;

	Axis _lockedAxis = Axis::None;

	bool _empty = true;
	bool _dirty = false;

	bool _extract = false;
	bool _extractCursor = false;
	bool _extractSelection = false;
	int _lastRaytraceX = -1;
	int _lastRaytraceY = -1;
	long _lastActionExecution = 0l;
	Action _lastAction = Action::None;
	// the action to execute on mouse move
	Action _action = Action::None;
	voxel::PickResult _result;

	void markExtract();
	void markCursorExtract();
	void modified(const voxel::Region& modifiedRegion, bool markUndo = true);
	bool placeCursor(voxel::Region* modifiedRegion);
	bool setVoxel(const glm::ivec3& pos, const voxel::Voxel& voxel);
	bool actionRequiresExistingVoxel(Action action) const;
public:
	Model();
	~Model();

	void onResize(const glm::ivec2& size);

	const glm::ivec3& cursorPosition() const;
	void setCursorPosition(glm::ivec3 pos, bool force = false);

	void init();
	void update();
	void shutdown();

	void copy();
	void paste();
	void cut();

	void crop();
	void extend(int size = 1);
	void scale();
	void fill(int x, int y, int z);

	bool importHeightmap(const std::string& file);
	bool save(const std::string& file);
	bool load(const std::string& file);

	bool newVolume(bool force);

	const voxel::Voxel& getVoxel(const glm::ivec3& pos) const;
	bool dirty() const;
	bool needExtract() const;
	bool empty() const;
	int size() const;

	void setNewVolume(voxel::RawVolume* volume);

	void rotate(int angleX, int angleY, int angleZ);
	void move(int x, int y, int z);

	void render(const video::Camera& camera);
	void renderSelection(const video::Camera& camera);

	bool renderAxis() const;
	Action evalAction() const;
	Action action() const;
	void setAction(Action action);
	Action keyAction() const;
	Action uiAction() const;

	voxel::RawVolume* modelVolume();
	const voxel::RawVolume* modelVolume() const;

	voxel::RawVolume* cursorPositionVolume();
	const voxel::RawVolume* cursorPositionVolume() const;

	frontend::RawVolumeRenderer& rawVolumeRenderer();
	const frontend::RawVolumeRenderer& rawVolumeRenderer() const;

	frontend::RawVolumeRenderer& rawVolumeSelectionRenderer();
	const frontend::RawVolumeRenderer& rawVolumeSelectionRenderer() const;

	voxel::PickResult& result();

	void noise(int octaves, float persistence, float frequency, float amplitude);
	void lsystem(const voxel::lsystem::LSystemContext& lsystemCtx);
	void createTree(voxel::TreeContext ctx);
	void createBuilding(voxel::BuildingType type, const voxel::BuildingContext& ctx);
	void createPlant(voxel::PlantType type);
	void createCloud();
	void createCactus();
	void world(const voxel::WorldContext& ctx);

	bool extractVolume();
	bool extractCursorVolume();
	bool extractSelectionVolume();

	void setMousePos(int x, int y);

	bool trace(const video::Camera& camera);
	void select(const glm::ivec3& pos);
	void unselectAll();
	void executeAction(long now);
	void resetLastTrace();

	void setSelectionType(SelectType type);
	SelectType selectionType() const;

	Axis lockedAxis() const;
	void setLockedAxis(Axis axis, bool unlock);
	void updateAxisPlane(Axis axis);

	void undo();
	void redo();

	UndoHandler& undoHandler();
	const UndoHandler& undoHandler() const;

	void scaleCursorShape(const glm::vec3& scale);
	void setCursorShape(Shape shape);
	void setVoxel(const voxel::Voxel& voxel);
	ShapeHandler& shapeHandler();
	const ShapeHandler& shapeHandler() const;

public:
	// TODO: maybe move into scene
	bool _renderAxis = true;
	// the key action - has a higher priority than the ui action
	Action _keyAction = Action::None;
	// action that is selected via ui
	Action _uiAction = Action::PlaceVoxel;
	long _actionExecutionDelay = 20l;
};

inline Axis Model::lockedAxis() const {
	return _lockedAxis;
}

inline void Model::scaleCursorShape(const glm::vec3& scale) {
	_shapeHandler.scaleCursorShape(scale, cursorPositionVolume());
	resetLastTrace();
	markCursorExtract();
}

inline void Model::setVoxel(const voxel::Voxel& type) {
	_shapeHandler.setVoxel(type);
	voxel::RawVolume* cursorVolume = cursorPositionVolume();
	if (cursorVolume != nullptr) {
		_shapeHandler.setCursorShape(_shapeHandler.cursorShape(), cursorVolume, true);
	}
	markCursorExtract();
}

inline void Model::setCursorShape(Shape shape) {
	_shapeHandler.setCursorShape(shape, cursorPositionVolume(), true);
	resetLastTrace();
	markCursorExtract();
}

inline ShapeHandler& Model::shapeHandler() {
	return _shapeHandler;
}

inline const ShapeHandler& Model::shapeHandler() const {
	return _shapeHandler;
}

inline UndoHandler& Model::undoHandler() {
	return _undoHandler;
}

inline const UndoHandler& Model::undoHandler() const {
	return _undoHandler;
}

inline void Model::setSelectionType(SelectType type) {
	_selectionHandler.setSelectionType(type);
}

inline SelectType Model::selectionType() const {
	return _selectionHandler.selectionType();
}

inline voxel::PickResult& Model::result() {
	return _result;
}

inline bool Model::renderAxis() const {
	return _renderAxis;
}

inline Action Model::evalAction() const {
	if (_keyAction != Action::None) {
		return _keyAction;
	}
	if (action() != Action::None) {
		return action();
	}
	return _uiAction;
}

inline Action Model::action() const {
	return _action;
}

inline void Model::setAction(Action action) {
	_action = action;
}

inline Action Model::keyAction() const {
	return _keyAction;
}

inline voxel::RawVolume* Model::modelVolume() {
	return _rawVolumeRenderer.volume(ModelVolumeIndex);
}

inline const voxel::RawVolume* Model::modelVolume() const {
	return _rawVolumeRenderer.volume(ModelVolumeIndex);
}

inline voxel::RawVolume* Model::cursorPositionVolume() {
	return _rawVolumeRenderer.volume(CursorVolumeIndex);
}

inline const voxel::RawVolume* Model::cursorPositionVolume() const {
	return _rawVolumeRenderer.volume(CursorVolumeIndex);
}

inline Action Model::uiAction() const {
	return _uiAction;
}

inline frontend::RawVolumeRenderer& Model::rawVolumeRenderer() {
	return _rawVolumeRenderer;
}

inline const frontend::RawVolumeRenderer& Model::rawVolumeRenderer() const {
	return _rawVolumeRenderer;
}

inline frontend::RawVolumeRenderer& Model::rawVolumeSelectionRenderer() {
	return _rawVolumeSelectionRenderer;
}

inline const frontend::RawVolumeRenderer& Model::rawVolumeSelectionRenderer() const {
	return _rawVolumeSelectionRenderer;
}

inline bool Model::actionRequiresExistingVoxel(Action action) const {
	return action == Action::CopyVoxel || action == Action::DeleteVoxel
			|| action == Action::OverrideVoxel || action == Action::SelectVoxels;
}

inline bool Model::dirty() const {
	return _dirty;
}

inline bool Model::needExtract() const {
	return _extract;
}

inline int Model::size() const {
	return _size;
}

inline bool Model::empty() const {
	return _empty;
}

inline const glm::ivec3& Model::cursorPosition() const {
	return _cursorPos;
}

}
