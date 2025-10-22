#include "PhysicsEngine.h"
#include <algorithm>

PhysicsEngine::PhysicsEngine() 
    : gravity(0.0f, -9.81f, 0.0f)
    , airDensity(1.225f)
    , groundFriction(0.8f)
    , enableCollisions(true)
    , groundHeight(0.0f)
    , groundNormal(0.0f, 1.0f, 0.0f)
    , maxSubsteps(3)
    , fixedTimeStep(1.0f / 60.0f) {
}

PhysicsEngine::~PhysicsEngine() {
    clearCars();
}

void PhysicsEngine::addCar(Car* car) {
    if (car != nullptr) {
        cars.push_back(car);
    }
}

void PhysicsEngine::removeCar(Car* car) {
    if (car != nullptr) {
        cars.erase(std::remove(cars.begin(), cars.end(), car), cars.end());
    }
}

void PhysicsEngine::clearCars() {
    cars.clear();
}

void PhysicsEngine::setGravity(const Vector3& grav) {
    gravity = grav;
}

void PhysicsEngine::setAirDensity(float density) {
    airDensity = std::max(0.0f, density);
}

void PhysicsEngine::setGroundFriction(float friction) {
    groundFriction = std::max(0.0f, std::min(1.0f, friction));
}

void PhysicsEngine::setGroundHeight(float height) {
    groundHeight = height;
}

void PhysicsEngine::setGroundNormal(const Vector3& normal) {
    groundNormal = normal.normalized();
}

void PhysicsEngine::setEnableCollisions(bool enable) {
    enableCollisions = enable;
}

void PhysicsEngine::setMaxSubsteps(int steps) {
    maxSubsteps = std::max(1, steps);
}

void PhysicsEngine::setFixedTimeStep(float timeStep) {
    fixedTimeStep = std::max(0.001f, timeStep);
}

void PhysicsEngine::update(float deltaTime) {
    // Use fixed timestep for stable physics
    float remainingTime = deltaTime;
    int substeps = 0;
    
    while (remainingTime > 0.0f && substeps < maxSubsteps) {
        float currentTimeStep = std::min(remainingTime, fixedTimeStep);
        
        updateCars(currentTimeStep);
        
        if (enableCollisions) {
            updateCollisions();
        }
        
        remainingTime -= currentTimeStep;
        substeps++;
    }
}

void PhysicsEngine::updateCars(float deltaTime) {
    for (Car* car : cars) {
        if (car != nullptr) {
            updateGroundCollision(car);
            car->update(deltaTime);
        }
    }
}

void PhysicsEngine::updateCollisions() {
    // Simple collision detection between cars
    for (size_t i = 0; i < cars.size(); i++) {
        for (size_t j = i + 1; j < cars.size(); j++) {
            if (cars[i] != nullptr && cars[j] != nullptr) {
                Vector3 pos1 = cars[i]->getPosition();
                Vector3 pos2 = cars[j]->getPosition();
                
                float distance = (pos1 - pos2).length();
                float collisionRadius = 2.0f; // Car collision radius
                
                if (distance < collisionRadius * 2.0f) {
                    // Simple collision response
                    Vector3 collisionVector = (pos1 - pos2).normalized();
                    float overlap = collisionRadius * 2.0f - distance;
                    
                    // Separate cars
                    cars[i]->setPosition(pos1 + collisionVector * overlap * 0.5f);
                    cars[j]->setPosition(pos2 - collisionVector * overlap * 0.5f);
                    
                    // Apply collision forces (simplified)
                    Vector3 relativeVelocity = cars[i]->getVelocity() - cars[j]->getVelocity();
                    float collisionForce = relativeVelocity.dot(collisionVector);
                    
                    if (collisionForce > 0.0f) {
                        Vector3 impulse = collisionVector * collisionForce * 0.5f;
                        // Apply impulse to both cars (simplified)
                    }
                }
            }
        }
    }
}

void PhysicsEngine::updateGroundCollision(Car* car) {
    if (car == nullptr) return;
    
    Vector3 carPos = car->getPosition();
    
    // Simple ground plane collision
    if (carPos.y <= groundHeight) {
        car->setPosition(Vector3(carPos.x, groundHeight, carPos.z));
        
        // Apply ground friction and normal force
        Vector3 velocity = car->getVelocity();
        if (velocity.y < 0.0f) {
            velocity.y = 0.0f; // Stop downward velocity
        }
        
        // Apply ground friction
        Vector3 horizontalVelocity = Vector3(velocity.x, 0.0f, velocity.z);
        Vector3 frictionForce = -horizontalVelocity * groundFriction;
        velocity += frictionForce * 0.016f; // Assuming 60 FPS
        
        car->setVelocity(velocity);
    }
}

void PhysicsEngine::debugDraw() const {
    // This would be implemented with a debug rendering system
    // For now, it's a placeholder
}