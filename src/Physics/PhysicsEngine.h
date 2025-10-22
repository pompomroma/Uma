#pragma once
#include "Car.h"
#include "../Math/Vector3.h"
#include <vector>

class PhysicsEngine {
private:
    std::vector<Car*> cars;
    Vector3 gravity;
    float airDensity;
    float groundFriction;
    bool enableCollisions;
    
    // Ground properties
    float groundHeight;
    Vector3 groundNormal;
    
    // Performance settings
    int maxSubsteps;
    float fixedTimeStep;

public:
    PhysicsEngine();
    ~PhysicsEngine();
    
    // Car management
    void addCar(Car* car);
    void removeCar(Car* car);
    void clearCars();
    
    // Physics settings
    void setGravity(const Vector3& grav);
    void setAirDensity(float density);
    void setGroundFriction(float friction);
    void setGroundHeight(float height);
    void setGroundNormal(const Vector3& normal);
    void setEnableCollisions(bool enable);
    void setMaxSubsteps(int steps);
    void setFixedTimeStep(float timeStep);
    
    // Physics update
    void update(float deltaTime);
    void updateCars(float deltaTime);
    void updateCollisions();
    void updateGroundCollision(Car* car);
    
    // Utility functions
    Vector3 getGravity() const { return gravity; }
    float getAirDensity() const { return airDensity; }
    float getGroundFriction() const { return groundFriction; }
    bool getEnableCollisions() const { return enableCollisions; }
    
    // Debug
    void debugDraw() const;
};