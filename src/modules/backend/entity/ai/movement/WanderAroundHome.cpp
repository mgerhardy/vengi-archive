/**
 * @file
 */

#include "WanderAroundHome.h"
#include "backend/entity/ai/common/Math.h"
#include "math/Random.h"
#include "backend/entity/Npc.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace backend {

MoveVector WanderAroundHome::execute(const AIPtr& ai, float speed) const {
	backend::Npc& npc = getNpc(ai);
	const glm::vec3 target(npc.homePosition());
	const glm::vec3& pos = npc.pos();
	glm::vec3 v;
	if (glm::distance2(pos, target) > _maxDistance) {
		v = glm::normalize(target - pos);
	} else {
		v = glm::normalize(pos - target);
	}
	math::Random random;
	const float orientation = angle(v) + random.randomBinomial() * glm::radians(3.0f);
	const MoveVector d(v * speed, orientation);
	return d;
}

}
