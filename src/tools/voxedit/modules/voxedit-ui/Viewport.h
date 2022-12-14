/**
 * @file
 */

#pragma once

#include "video/Camera.h"
#include "video/FrameBuffer.h"
#include "imgui.h"
#include "video/WindowedApp.h"
#include "video/gl/GLTypes.h"
#include "video/Camera.h"
#include "video/FrameBuffer.h"
#include "voxedit-util/ViewportController.h"

namespace voxelformat {
class SceneGraphNode;
}

namespace voxedit {

class Viewport {
private:
	const core::String _id;
	bool _hovered = false;
	bool _hoveredGuizmoLastFrame = false;
	bool _guizmoActivated = false;
	struct Bounds {
		glm::vec3 mins{0};
		glm::vec3 maxs{0};
	};
	Bounds _boundsNode;
	Bounds _bounds;

	video::FrameBuffer _frameBuffer;
	video::TexturePtr _texture;
	voxedit::ViewportController _controller;
	core::VarPtr _modelSpaceVar;
	core::VarPtr _showAxisVar;
	core::VarPtr _guizmoRotation;
	core::VarPtr _guizmoAllowAxisFlip;
	core::VarPtr _guizmoSnap;

	void renderToFrameBuffer();
	bool setupFrameBuffer(const glm::ivec2& frameBufferSize);
	void renderSceneGuizmo(video::Camera &camera);
	void renderCameraManipulator(video::Camera &camera);
	void renderGizmo(video::Camera &camera, const float headerSize, const ImVec2 &size);
	void updateViewportTrace(float headerSize);
public:
	Viewport(const core::String& id);
	~Viewport();

	bool isHovered() const;
	void update();
	bool init(ViewportController::RenderMode renderMode = ViewportController::RenderMode::Editor);
	void shutdown();

	const core::String& id() const;

	void resetCamera();
	bool saveImage(const char* filename);

	video::FrameBuffer& frameBuffer();
	video::Camera& camera();
	ViewportController& controller();
};

inline const core::String& Viewport::id() const {
	return _id;
}

inline bool Viewport::isHovered() const {
	return _hovered;
}

inline video::FrameBuffer& Viewport::frameBuffer() {
	return _frameBuffer;
}

inline ViewportController& Viewport::controller() {
	return _controller;
}

inline video::Camera& Viewport::camera() {
	return _controller.camera();
}

}
