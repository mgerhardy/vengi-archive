/**
 * @file LUAAIRegistry.h
 * @ingroup LUA
 */

#pragma once

#include "AIRegistry.h"
#include "core/Trace.h"
#include "core/concurrent/Concurrency.h"
#include "core/concurrent/Lock.h"
#include "commonlua/LUA.h"
#include "backend/entity/ai/tree/LUATreeNode.h"
#include "backend/entity/ai/condition/LUACondition.h"
#include "backend/entity/ai/filter/LUAFilter.h"
#include "backend/entity/ai/movement/LUASteering.h"
#include <map>

namespace backend {

using LuaNodeFactory = LUATreeNode::LUATreeNodeFactory;
typedef std::shared_ptr<LuaNodeFactory> LUATreeNodeFactoryPtr;
typedef std::map<core::String, LUATreeNodeFactoryPtr> TreeNodeFactoryMap;

using LuaConditionFactory = LUACondition::LUAConditionFactory;
typedef std::shared_ptr<LuaConditionFactory> LUAConditionFactoryPtr;
typedef std::map<core::String, LUAConditionFactoryPtr> ConditionFactoryMap;

using LuaFilterFactory = LUAFilter::LUAFilterFactory;
typedef std::shared_ptr<LuaFilterFactory> LUAFilterFactoryPtr;
typedef std::map<core::String, LUAFilterFactoryPtr> FilterFactoryMap;

using LuaSteeringFactory = movement::LUASteering::LUASteeringFactory;
typedef std::shared_ptr<LuaSteeringFactory> LUASteeringFactoryPtr;
typedef std::map<core::String, LUASteeringFactoryPtr> SteeringFactoryMap;

/**
 * @brief Allows you to register lua @ai{TreeNode}s, @ai{Conditions}, @ai{Filter}s and @ai{ISteering}s.
 *
 * @see @ai{LUATreeNode}
 *
 * @par TreeNode
 * @code
 * local luanode = REGISTRY.createNode("SomeName")
 * function luanode:execute(ai, deltaMillis)
 *   print("Node execute called with parameters: ai=["..tostring(ai).."], deltaMillis=["..tostring(deltaMillis).."]")
 *   return FINISHED
 * end
 * @encode
 * The @ai{TreeNodeStatus} states are put into the global space. They are: @c UNKNOWN, @c CANNOTEXECUTE,
 * @c RUNNING, @c FINISHED, @c FAILED and @c EXCEPTION
 *
 * Use @c SomeName later on in your behaviour trees to use this @ai{ITreeNode}
 *
 * @par Conditions
 * @code
 * local luacondition = REGISTRY.createCondition("SomeName")
 * function luacondition:evaluate(ai)
 *   --print("Condition evaluate called with parameter: ai=["..tostring(ai).."]")
 *   return true
 * end
 * @encode
 *
 * Use @c SomeName later on in your behaviour trees to use this @ai{ICondition}
 *
 * @par IFilter
 * @code
 * local luafilter = REGISTRY.createFilter("SomeName")
 * function luafilter:filter(ai)
 * end
 * @endcode
 *
 * Use @c SomeName later on in your behaviour trees to use this @ai{ICondition}
 *
 * @par ISteering
 * @code
 * local luasteering = REGISTRY.createSteering("SomeName")
 * function luasteering:execute(ai, speed)
 *   -- return MoveVector
 *   return 0.0, 1.0, 0.0, 0.6
 * end
 * @endcode
 *
 * Use @c SomeName later on in your behaviour trees to use this @ai{ICondition}
 *
 * @par AI metatable
 * There is a metatable that you can modify by calling @ai{LUAAIRegistry::pushAIMetatable()}.
 * This metatable is applied to all @ai{AI} pointers that are forwarded to the lua functions.
 */
class LUAAIRegistry : public AIRegistry {
protected:
	lua::LUA _lua;
	lua_State* _s = nullptr;

	core_trace_mutex(core::Lock, _lock, "LUAAIRegistry");
	TreeNodeFactoryMap _treeNodeFactories core_thread_guarded_by(_lock);
	ConditionFactoryMap _conditionFactories core_thread_guarded_by(_lock);
	FilterFactoryMap _filterFactories core_thread_guarded_by(_lock);
	SteeringFactoryMap _steeringFactories core_thread_guarded_by(_lock);
public:
	LUAAIRegistry();

	void addTreeNodeFactory(const core::String& type, const LUATreeNodeFactoryPtr& factory);
	void addConditionFactory(const core::String& type, const LUAConditionFactoryPtr& factory);
	void addFilterFactory(const core::String& type, const LUAFilterFactoryPtr& factory);
	void addSteeringFactory(const core::String& type, const LUASteeringFactoryPtr& factory);

	/**
	 * @brief Access to the lua state.
	 * @see pushAIMetatable()
	 */
	lua_State* getLuaState();

	/**
	 * @brief Pushes the AI metatable onto the stack. This allows anyone to modify it
	 * to provide own functions and data that is applied to the @c ai parameters of the
	 * lua functions.
	 * @note lua_ctxai() can be used in your lua c callbacks to get access to the
	 * @ai{AI} pointer: @code const AI* ai = lua_ctxai(s, 1); @endcode
	 */
	int pushAIMetatable();

	/**
	 * @brief Pushes the character metatable onto the stack. This allows anyone to modify it
	 * to provide own functions and data that is applied to the @c ai:character() value
	 */
	int pushCharacterMetatable();

	/**
	 * @see shutdown()
	 */
	bool init(const core::String& file = "behaviourtreenodes.lua");

	/**
	 * @see init()
	 */
	void shutdown();

	~LUAAIRegistry();

	inline bool evaluate(const core::String& str) {
		return evaluate(str.c_str(), str.size());
	}

	/**
	 * @brief Load your lua scripts into the lua state of the registry.
	 * This can be called multiple times to e.g. load multiple files.
	 * @return @c true if the lua script was loaded, @c false otherwise
	 * @note you have to call init() before
	 */
	bool evaluate(const char* luaBuffer, size_t size);
};

typedef std::shared_ptr<LUAAIRegistry> AIRegistryPtr;

}
