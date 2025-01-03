// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.hitBoxScale.x*.9), abs(motion.hitBoxScale.y*.9) };
}

vec2 get_bounding_box_scale(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

struct Triangle {
	glm::vec2 v0, v1, v2;
};


std::vector<Triangle> transformMeshToWorld(const std::vector<ColoredVertex>& vertices,
	const std::vector<uint16_t>& indices,
	const Motion& motion) {
	std::vector<Triangle> transformedTriangles;


	glm::mat3 transform = glm::mat4(1.0f);
	glm::vec2 center = motion.position;

	glm::mat3 scaleMatrix = glm::scale(glm::mat3(1.0f), motion.scale);
	glm::mat3 rotateMatrix = glm::rotate(glm::mat3(1.0f), motion.angle);
	glm::mat3 translateMatrix = glm::translate(glm::mat3(1.0f), center);

	transform = translateMatrix * rotateMatrix * scaleMatrix;

	std::vector<glm::vec2> transformedVertices(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i) {

		glm::vec3 centeredVertex = glm::vec3(vertices[i].position) - glm::vec3(motion.position, 0.0f);
		transformedVertices[i] = transform * centeredVertex;
	}

	for (size_t i = 0; i < indices.size(); i += 3) {
		Triangle triangle;
		triangle.v0 = transformedVertices[indices[i]];
		triangle.v1 = transformedVertices[indices[i + 1]];
		triangle.v2 = transformedVertices[indices[i + 2]];
		transformedTriangles.push_back(triangle);
	}

	return transformedTriangles;
}


bool triangleIntersectsBoundingBox(const Triangle& triangle, const Motion& motion) {
	vec2 halfSize = get_bounding_box(motion) / 2.f;
	float left = motion.position.x - halfSize.x;
	float right = motion.position.x + halfSize.x;
	float top = motion.position.y + halfSize.y;
	float bottom = motion.position.y - halfSize.y;

	std::vector<glm::vec2> points = { triangle.v0, triangle.v1, triangle.v2 };

	for (const auto& point : points) {
		if (point.x >= left && point.x <= right && point.y >= bottom && point.y <= top) {
			return true;
		}
	}

	if ((triangle.v0.x < left && triangle.v1.x < left && triangle.v2.x < left) ||
		(triangle.v0.x > right && triangle.v1.x > right && triangle.v2.x > right) ||
		(triangle.v0.y < bottom && triangle.v1.y < bottom && triangle.v2.y < bottom) ||
		(triangle.v0.y > top && triangle.v1.y > top && triangle.v2.y > top)) {
		return false;
	}

	return false;
}

bool meshIntersectsMotion(const std::vector<ColoredVertex>& vertices,
	const std::vector<uint16_t>& indices,
	const Motion& motion) {

	auto transformedTriangles = transformMeshToWorld(vertices, indices, motion);

	for (const auto& triangle : transformedTriangles) {
		if (triangleIntersectsBoundingBox(triangle, motion)) {
			return true;
		}
	}

	return false;
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp, dp);
	vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	other_bonding_box = { other_bonding_box.x, other_bonding_box.y };
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	my_bonding_box = { my_bonding_box.x, my_bonding_box.y };
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// Using this for inspiration https://www.jeffreythompson.org/collision-detection/circle-rect.php
bool circleIntersectsRectangle(const Motion& circle, const Motion& rectangle) {
	// Extract circle properties
	float cx = circle.position.x;
	float cy = circle.position.y;
	float radius = std::max(abs(circle.hitBoxScale.x), abs(circle.hitBoxScale.y)) / 2.f;

	vec2 halfSize = get_bounding_box(rectangle) / 2.f;
	float rx = rectangle.position.x - halfSize.x; // left edge
	float ry = rectangle.position.y - halfSize.y; // bottom edge
	float rw = 2.f * halfSize.x;                 // width
	float rh = 2.f * halfSize.y;                 // height

	// Find the closest point on the rectangle to the circle center
	float testX = cx;
	float testY = cy;

	if (cx < rx)         testX = rx;        // left edge
	else if (cx > rx + rw) testX = rx + rw;   // right edge
	if (cy < ry)         testY = ry;        // bottom edge
	else if (cy > ry + rh) testY = ry + rh;   // top edge

	// Calculate distance from circle center to closest point
	float distX = cx - testX;
	float distY = cy - testY;
	float distanceSquared = distX * distX + distY * distY;

	// Check if the distance is less than or equal to the radius
	return distanceSquared <= radius * radius;
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move HERO based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		float step_seconds = elapsed_ms / 1000.f;
		motion.position += step_seconds * motion.velocity;
		//motion.position.x -= step_seconds * motion.velocity.x;
		//motion.position.y += step_seconds * motion.velocity.y;
		(void)elapsed_ms; // placeholder to silence unused warning until implemented
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			Entity entity_j = motion_container.entities[j];
			Motion motion_j = motion_container.components[j];
			if ((registry.renderRequestsMap.has(entity_j) && registry.renderRequestsMap.has(entity_i)) || 
			(registry.renderRequests.has(entity_j) && registry.renderRequests.has(entity_i))) {
				if (registry.meshPtrs.has(entity_i)) {
					Motion motion_i = registry.motions.get(entity_i);
					Mesh* mesh = registry.meshPtrs.get(entity_i);

					if (meshIntersectsMotion(mesh->vertices, mesh->vertex_indices, motion_i)) {
						registry.collisions.emplace_with_duplicates(entity_i, entity_j);
						registry.collisions.emplace_with_duplicates(entity_j, entity_i);
					}
				}
				if (registry.walls.has(entity_j) && !(registry.mapEnemies.has(entity_i))) {
					Motion motion_i = registry.motions.get(entity_i);
					if (circleIntersectsRectangle(motion_i, motion_j)) {
						registry.collisions.emplace_with_duplicates(entity_i, entity_j);
						registry.collisions.emplace_with_duplicates(entity_j, entity_i);
					}
				} else if (registry.walls.has(entity_i) && !registry.mapEnemies.has(entity_j)) {
					Motion motion_i = registry.motions.get(entity_i);
					if (circleIntersectsRectangle(motion_j, motion_i)) {
						registry.collisions.emplace_with_duplicates(entity_i, entity_j);
						registry.collisions.emplace_with_duplicates(entity_j, entity_i);
					}
				}
				else if (collides(motion_i, motion_j))
				{
					if (registry.walls.has(entity_j) && registry.mapEnemies.has(entity_i)) {
						registry.remove_all_components_of(entity_i);
					} 
					else if (registry.walls.has(entity_i) && registry.mapEnemies.has(entity_j)) {
						registry.remove_all_components_of(entity_j);
					}
					else {
						registry.collisions.emplace_with_duplicates(entity_i, entity_j);
						registry.collisions.emplace_with_duplicates(entity_j, entity_i);
					}
					// Create a collisions event
					// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				}
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}