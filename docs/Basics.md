# Basics

## Build

See [compilation](Compilation.md) and [dependencies](Dependencies.md).

## Coordinate system

We are using the default opengl right handed coordinate system with x going right, y going upwards and z point towards you.

We are using column major matrices.

![image](img/coordinate_system.png)

## Unittests

If you are going to contribute, make sure that you are adding unittests, too. I won't make promises about not breaking anything
if there aren't unittests that are telling me that I've broken something.

To add a unittest, each module (`src/modules/XXX`) has a `tests/` subdirectory. The `CMakeLists.txt` in the module directory adds
the source files from that folder.

```cmake
set(TEST_SRCS
  [...]
)
gtest_suite_sources(tests ${TEST_SRCS})
gtest_suite_deps(tests ${LIB})

gtest_suite_begin(tests-${LIB} TEMPLATE ${ROOT_DIR}/src/modules/core/tests/main.cpp.in)
gtest_suite_sources(tests-${LIB} ${TEST_SRCS})
gtest_suite_deps(tests-${LIB} ${LIB})
gtest_suite_end(tests-${LIB})
```

This adds your tests to the global `tests` binary but also to a dedicated `tests-XXX` binary.

## Coding style

Rule of thumb - stick to the existing coding style - you can also use the `clang-format` settings to format your code. In general
you should not include any whitespace or formatting changes if they don't belong to your code changes.

If you do a formatting change, this should not be mixed with code changes - make a dedicated commit for the formatting.

Avoid using the STL were possible - see [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b).

## Commit messages

Commit messages should match the usual git commit message guidelines. Keep the summary short - put an UPPERCASE prefix in front
of it and try to explain why the change was made - not what you changed (because that is part of the commit diff already).

The prefix is usually the module name. E.g. if you are changing code in `src/modules/voxelformat` the prefix would be `VOXELFORMAT`. A commit message could look like this:

```
VOXELFORMAT: summary

detail message line 1
detail message line 2
```

## Modules

| Name             | Description                                                                |
| ---------------- | -------------------------------------------------------------------------- |
| ai-shared        | Protocol for the [ai debugger](AIRemoteDebugger.md)                        |
| animation        | [Skeletal animation](Animations.md) with lua configuration support         |
| app              | Basic application classes                                                  |
| attrib           | (GAME) [Attributes](Attributes.md) for characters, items, ...              |
| audio            | Sound and music module                                                     |
| backend          | (GAME) Game server backend module                                          |
| command          | Bind c++ functionality to console commands                                 |
| commonlua        | Basic [lua](LUAScript.md) bindings and helper                              |
| compute          | [OpenCL](ComputeShaderTool.md) compute shaders                             |
| computevideo     | [OpenCL](ComputeShaderTool.md) compute shaders                             |
| console          | Base classes for different kind of consoles                                |
| cooldown         | (GAME) Cooldown management module                                          |
| core             | String, collections and other foundation classes                           |
| eventmgr         | (GAME) Game events module                                                  |
| frontend         | (GAME) Game frontend module                                                |
| http             | HTTP server and client                                                     |
| image            | Image loading and writing                                                  |
| io               | Stream and file handling                                                   |
| math             | Based on glm                                                               |
| metric           | telegraf, influx and other metrics                                         |
| network          | Network module based on enet and protocol based on flatbuffers             |
| noise            | Different noise implementations                                            |
| persistence      | [Persistency](Persistence.md)                                              |
| poi              | Point of interests                                                         |
| render           | General renderer implementations and helpers                               |
| rma              | Random map assembly                                                        |
| shared           | (GAME) Protocol and shared code between `backend` and `frontend`           |
| stock            | (GAME) Stock handling for entities                                         |
| testcore         | Visual test helpers                                                        |
| ui               | DearImgui based ui code                                                    |
| util             |                                                                            |
| uuidutil         |                                                                            |
| video            | [Window and renderer](ShaderTool.md) module                                |
| voxel            | The voxel engine code based on PolyVox                                     |
| voxelfont        | TTF font to voxel                                                          |
| voxelformat      | Several volume and mesh based file formats to load or generate voxels      |
| voxelgenerator   | LUA generator, space colonization, tree- and shape generators              |
| voxelrender      | Voxel renderer                                                             |
| voxelutil        | Pathfinding, raycasting, image and general util functions                  |
| voxelworld       | Streaming voxel world module                                               |
| voxelworldrender | Renderer for `voxelworld`                                                  |
