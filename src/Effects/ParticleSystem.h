#pragma once
#include "../Math/Vector3.h"
#include "../Combat/Projectile.h"
#include <vector>
#include <memory>
#include <functional>

class Renderer;

class ParticleSystem {
public:
    struct Particle {
        Vector3 position;
        Vector3 velocity;
        Vector3 acceleration;
        Vector3 color;
        float size;
        float life;
        float maxLife;
        float rotation;
        float angularVelocity;
        bool active;
        
        Particle() : position(Vector3::zero()), velocity(Vector3::zero()), 
                    acceleration(Vector3::zero()), color(1.0f, 1.0f, 1.0f),
                    size(1.0f), life(1.0f), maxLife(1.0f), rotation(0.0f),
                    angularVelocity(0.0f), active(false) {}
    };

    enum class EffectType {
        LaserImpact,        // Small sparks
        PlasmaExplosion,    // Medium explosion with plasma effects
        MissileExplosion,   // Large explosion with fire and smoke
        EnergyBurst,        // Radial energy waves
        Shield,             // Shimmering shield effect
        Teleport,           // Teleportation particles
        Dash,               // Speed trail particles
        Heal,               // Green healing particles
        Damage,             // Red damage particles
        LevelUp,            // Golden celebration particles
        Boost,              // Engine boost particles
        Smoke,              // General smoke effect
        Fire,               // Fire particles
        Sparks,             // Metal sparks
        Magic               // Magical energy particles
    };

private:
    Renderer* renderer;
    std::vector<Particle> particles;
    int maxParticles;
    int activeParticleCount;
    
    // Effect templates
    struct EffectTemplate {
        int particleCount;
        float particleLife;
        Vector3 baseColor;
        float baseSize;
        Vector3 velocityRange;
        Vector3 accelerationRange;
        float sizeVariation;
        float lifeVariation;
        bool useGravity;
        bool fadeOut;
        bool scaleDown;
        
        EffectTemplate() : particleCount(50), particleLife(2.0f), 
                          baseColor(1.0f, 1.0f, 1.0f), baseSize(1.0f),
                          velocityRange(5.0f, 5.0f, 5.0f), 
                          accelerationRange(0.0f, -9.81f, 0.0f),
                          sizeVariation(0.5f), lifeVariation(0.5f),
                          useGravity(true), fadeOut(true), scaleDown(false) {}
    };
    
    std::vector<EffectTemplate> effectTemplates;

public:
    ParticleSystem(Renderer* renderer, int maxParticles = 10000);
    ~ParticleSystem();
    
    // System management
    void initialize();
    void shutdown();
    void update(float deltaTime);
    void render();
    void clear();
    
    // Effect creation
    void createEffect(EffectType type, const Vector3& position, const Vector3& direction = Vector3::up());
    void createExplosion(const Vector3& position, float radius, Projectile::ProjectileType weaponType);
    void createProjectileTrail(const Vector3& position, const Vector3& velocity, Projectile::ProjectileType projectileType);
    void createShieldEffect(const Vector3& position, float radius);
    void createTeleportEffect(const Vector3& fromPos, const Vector3& toPos);
    void createDashEffect(const Vector3& position, const Vector3& direction);
    void createHealEffect(const Vector3& position);
    void createDamageEffect(const Vector3& position, float damage);
    void createLevelUpEffect(const Vector3& position);
    void createBoostEffect(const Vector3& position, const Vector3& direction);
    
    // Particle management
    void addParticle(const Particle& particle);
    void addParticles(const std::vector<Particle>& newParticles);
    int getActiveParticleCount() const { return activeParticleCount; }
    int getMaxParticles() const { return maxParticles; }
    
    // Settings
    void setMaxParticles(int max);
    
private:
    void initializeEffectTemplates();
    void updateParticles(float deltaTime);
    void removeInactiveParticles();
    Particle createParticle(const EffectTemplate& tmpl, const Vector3& position, const Vector3& direction);
    Vector3 randomizeVector(const Vector3& base, const Vector3& range);
    float randomizeFloat(float base, float variation);
    Vector3 getRandomDirection();
    Vector3 getRandomDirectionInCone(const Vector3& direction, float coneAngle);
    EffectTemplate& getEffectTemplate(EffectType type);
};