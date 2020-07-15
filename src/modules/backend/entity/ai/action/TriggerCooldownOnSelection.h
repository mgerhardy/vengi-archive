/**
 * @file
 */

#pragma once

#include "Task.h"
#include "cooldown/CooldownType.h"
#include "backend/entity/EntityStorage.h"

namespace backend {

/**
 * @ingroup AI
 */
class TriggerCooldownOnSelection: public Task {
private:
	cooldown::Type _cooldownId;
public:
	TriggerCooldownOnSelection(const core::String& name, const core::String& parameters, const ConditionPtr& condition);
	NODE_FACTORY(TriggerCooldownOnSelection)

	ai::TreeNodeStatus doAction(AICharacter& chr, int64_t deltaMillis) override;
};

}

