#include "ParticleSystem.h"
#include "../Rendering/Renderer.h"
#include <algorithm>
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(Renderer* renderer, int maxParticles)
    : renderer(renderer)
    , maxParticles(maxParticles)
    , activeParticleCount(0) {
    
    particles.reserve(maxParticles);
    initializeEffectTemplates();
}

ParticleSystem::~ParticleSystem() {
    shutdown();
}

void ParticleSystem::initialize() {
    particles.clear();
    particles.resize(maxParticles);
    activeParticleCount = 0;
}

void ParticleSystem::shutdown() {
    particles.clear();
    activeParticleCount = 0;
}

void ParticleSystem::update(float deltaTime) {
    updateParticles(deltaTime);
    removeInactiveParticles();
}

void ParticleSystem::render() {
    if (!renderer) return;
    
    // Render all active particles
    for (const auto& particle : particles) {
        if (particle.active) {
            // This would use the renderer to draw each particle
            // renderer->renderParticle(particle.position, particle.size, particle.color, particle.rotation);
        }
    }
}

void ParticleSystem::clear() {
    for (auto& particle : particles) {
        particle.active = false;
    }
    activeParticleCount = 0;
}

void ParticleSystem::initializeEffectTemplates() {
    effectTemplates.resize(static_cast<int>(EffectType::Magic) + 1);
    
    // Laser Impact
    auto& laserImpact = effectTemplates[static_cast<int>(EffectType::LaserImpact)];
    laserImpact.particleCount = 15;
    laserImpact.particleLife = 0.5f;
    laserImpact.baseColor = Vector3(1.0f, 0.2f, 0.2f);
    laserImpact.baseSize = 0.3f;
    laserImpact.velocityRange = Vector3(8.0f, 8.0f, 8.0f);
    laserImpact.lifeVariation = 0.3f;
    laserImpact.fadeOut = true;
    
    // Plasma Explosion
    auto& plasmaExplosion = effectTemplates[static_cast<int>(EffectType::PlasmaExplosion)];
    plasmaExplosion.particleCount = 40;
    plasmaExplosion.particleLife = 1.5f;
    plasmaExplosion.baseColor = Vector3(0.2f, 0.8f, 1.0f);
    plasmaExplosion.baseSize = 0.8f;
    plasmaExplosion.velocityRange = Vector3(12.0f, 12.0f, 12.0f);
    plasmaExplosion.sizeVariation = 0.6f;
    plasmaExplosion.fadeOut = true;
    plasmaExplosion.scaleDown = true;
    
    // Missile Explosion
    auto& missileExplosion = effectTemplates[static_cast<int>(EffectType::MissileExplosion)];
    missileExplosion.particleCount = 80;
    missileExplosion.particleLife = 2.5f;
    missileExplosion.baseColor = Vector3(1.0f, 0.5f, 0.1f);
    missileExplosion.baseSize = 1.2f;
    missileExplosion.velocityRange = Vector3(15.0f, 15.0f, 15.0f);
    missileExplosion.sizeVariation = 0.8f;
    missileExplosion.lifeVariation = 0.7f;
    missileExplosion.fadeOut = true;
    missileExplosion.scaleDown = true;
    
    // Energy Burst
    auto& energyBurst = effectTemplates[static_cast<int>(EffectType::EnergyBurst)];
    energyBurst.particleCount = 60;
    energyBurst.particleLife = 1.0f;
    energyBurst.baseColor = Vector3(0.8f, 0.2f, 1.0f);
    energyBurst.baseSize = 0.6f;
    energyBurst.velocityRange = Vector3(20.0f, 5.0f, 20.0f);
    energyBurst.useGravity = false;
    energyBurst.fadeOut = true;
    
    // Shield
    auto& shield = effectTemplates[static_cast<int>(EffectType::Shield)];
    shield.particleCount = 30;
    shield.particleLife = 0.8f;
    shield.baseColor = Vector3(0.3f, 0.8f, 1.0f);
    shield.baseSize = 0.4f;
    shield.velocityRange = Vector3(3.0f, 3.0f, 3.0f);
    shield.useGravity = false;
    shield.fadeOut = true;
    
    // Teleport
    auto& teleport = effectTemplates[static_cast<int>(EffectType::Teleport)];
    teleport.particleCount = 50;
    teleport.particleLife = 1.2f;
    teleport.baseColor = Vector3(1.0f, 1.0f, 0.3f);
    teleport.baseSize = 0.5f;
    teleport.velocityRange = Vector3(10.0f, 15.0f, 10.0f);
    teleport.useGravity = false;
    teleport.fadeOut = true;
    
    // Dash
    auto& dash = effectTemplates[static_cast<int>(EffectType::Dash)];
    dash.particleCount = 25;
    dash.particleLife = 0.6f;
    dash.baseColor = Vector3(0.8f, 0.8f, 1.0f);
    dash.baseSize = 0.3f;
    dash.velocityRange = Vector3(5.0f, 2.0f, 5.0f);
    dash.fadeOut = true;
    
    // Heal
    auto& heal = effectTemplates[static_cast<int>(EffectType::Heal)];
    heal.particleCount = 35;
    heal.particleLife = 2.0f;
    heal.baseColor = Vector3(0.2f, 1.0f, 0.2f);
    heal.baseSize = 0.4f;
    heal.velocityRange = Vector3(3.0f, 8.0f, 3.0f);
    heal.accelerationRange = Vector3(0.0f, -2.0f, 0.0f);
    heal.fadeOut = true;
    
    // Damage
    auto& damage = effectTemplates[static_cast<int>(EffectType::Damage)];
    damage.particleCount = 20;
    damage.particleLife = 0.8f;
    damage.baseColor = Vector3(1.0f, 0.1f, 0.1f);
    damage.baseSize = 0.3f;
    damage.velocityRange = Vector3(6.0f, 8.0f, 6.0f);
    damage.fadeOut = true;
    
    // Level Up
    auto& levelUp = effectTemplates[static_cast<int>(EffectType::LevelUp)];
    levelUp.particleCount = 100;
    levelUp.particleLife = 3.0f;
    levelUp.baseColor = Vector3(1.0f, 0.8f, 0.2f);
    levelUp.baseSize = 0.6f;
    levelUp.velocityRange = Vector3(8.0f, 15.0f, 8.0f);
    levelUp.useGravity = false;
    levelUp.fadeOut = true;
    levelUp.scaleDown = true;
}

void ParticleSystem::createEffect(EffectType type, const Vector3& position, const Vector3& direction) {
    EffectTemplate& tmpl = getEffectTemplate(type);
    
    for (int i = 0; i < tmpl.particleCount; i++) {
        if (activeParticleCount >= maxParticles) break;
        
        Particle particle = createParticle(tmpl, position, direction);
        addParticle(particle);
    }
}

void ParticleSystem::createExplosion(const Vector3& position, float radius, Projectile::ProjectileType weaponType) {
    EffectType effectType;
    
    switch (weaponType) {
        case Projectile::ProjectileType::Laser:
            effectType = EffectType::LaserImpact;
            break;
        case Projectile::ProjectileType::Plasma:
            effectType = EffectType::PlasmaExplosion;
            break;
        case Projectile::ProjectileType::Missile:
            effectType = EffectType::MissileExplosion;
            break;
        case Projectile::ProjectileType::EnergyBall:
            effectType = EffectType::EnergyBurst;
            break;
        default:
            effectType = EffectType::Sparks;
            break;
    }
    
    createEffect(effectType, position);
    
    // Add additional smoke for larger explosions
    if (radius > 3.0f) {
        createEffect(EffectType::Smoke, position);
    }
}

void ParticleSystem::createProjectileTrail(const Vector3& position, const Vector3& velocity, Projectile::ProjectileType projectileType) {
    // Create trail particles based on projectile type
    EffectType trailType = EffectType::Sparks;
    
    switch (projectileType) {
        case Projectile::ProjectileType::Laser:
            trailType = EffectType::LaserImpact;
            break;
        case Projectile::ProjectileType::Plasma:
            trailType = EffectType::Magic;
            break;
        case Projectile::ProjectileType::Missile:
            trailType = EffectType::Fire;
            break;
        case Projectile::ProjectileType::EnergyBall:
            trailType = EffectType::Magic;
            break;
        default:
            return; // No trail for fist attacks
    }
    
    // Create fewer particles for trails
    EffectTemplate& tmpl = getEffectTemplate(trailType);
    int originalCount = tmpl.particleCount;
    tmpl.particleCount = 3; // Reduce particle count for trails
    
    Vector3 trailDirection = -velocity.normalized();
    createEffect(trailType, position, trailDirection);
    
    tmpl.particleCount = originalCount; // Restore original count
}

void ParticleSystem::createShieldEffect(const Vector3& position, float radius) {
    createEffect(EffectType::Shield, position);
}

void ParticleSystem::createTeleportEffect(const Vector3& fromPos, const Vector3& toPos) {
    createEffect(EffectType::Teleport, fromPos);
    createEffect(EffectType::Teleport, toPos);
}

void ParticleSystem::createDashEffect(const Vector3& position, const Vector3& direction) {
    createEffect(EffectType::Dash, position, direction);
}

void ParticleSystem::createHealEffect(const Vector3& position) {
    createEffect(EffectType::Heal, position);
}

void ParticleSystem::createDamageEffect(const Vector3& position, float damage) {
    createEffect(EffectType::Damage, position);
}

void ParticleSystem::createLevelUpEffect(const Vector3& position) {
    createEffect(EffectType::LevelUp, position);
}

void ParticleSystem::createBoostEffect(const Vector3& position, const Vector3& direction) {
    createEffect(EffectType::Boost, position, direction);
}

void ParticleSystem::addParticle(const Particle& particle) {
    if (activeParticleCount >= maxParticles) return;
    
    // Find an inactive particle slot
    for (auto& p : particles) {
        if (!p.active) {
            p = particle;
            p.active = true;
            activeParticleCount++;
            break;
        }
    }
}

void ParticleSystem::updateParticles(float deltaTime) {
    for (auto& particle : particles) {
        if (!particle.active) continue;
        
        // Update life
        particle.life -= deltaTime;
        if (particle.life <= 0.0f) {
            particle.active = false;
            continue;
        }
        
        // Update physics
        particle.velocity += particle.acceleration * deltaTime;
        particle.position += particle.velocity * deltaTime;
        particle.rotation += particle.angularVelocity * deltaTime;
        
        // Update visual properties based on life
        float lifeRatio = particle.life / particle.maxLife;
        
        if (getEffectTemplate(EffectType::LaserImpact).fadeOut) { // Using as example - would need proper template reference
            particle.color = particle.color * lifeRatio;
        }
        
        if (getEffectTemplate(EffectType::LaserImpact).scaleDown) {
            particle.size = particle.size * lifeRatio;
        }
    }
}

void ParticleSystem::removeInactiveParticles() {
    activeParticleCount = 0;
    for (const auto& particle : particles) {
        if (particle.active) {
            activeParticleCount++;
        }
    }
}

ParticleSystem::Particle ParticleSystem::createParticle(const EffectTemplate& tmpl, const Vector3& position, const Vector3& direction) {
    Particle particle;
    
    particle.position = position;
    particle.life = randomizeFloat(tmpl.particleLife, tmpl.lifeVariation);
    particle.maxLife = particle.life;
    particle.color = tmpl.baseColor;
    particle.size = randomizeFloat(tmpl.baseSize, tmpl.sizeVariation);
    
    // Set velocity
    if (direction.length() > 0.001f) {
        particle.velocity = getRandomDirectionInCone(direction, 45.0f) * randomizeFloat(tmpl.velocityRange.length(), 0.3f);
    } else {
        particle.velocity = randomizeVector(Vector3::zero(), tmpl.velocityRange);
    }
    
    // Set acceleration
    particle.acceleration = randomizeVector(tmpl.accelerationRange, Vector3(1.0f, 1.0f, 1.0f));
    
    // Set rotation
    particle.rotation = randomizeFloat(0.0f, 360.0f);
    particle.angularVelocity = randomizeFloat(0.0f, 180.0f);
    
    particle.active = true;
    
    return particle;
}

Vector3 ParticleSystem::randomizeVector(const Vector3& base, const Vector3& range) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    return Vector3(
        base.x + dis(gen) * range.x,
        base.y + dis(gen) * range.y,
        base.z + dis(gen) * range.z
    );
}

float ParticleSystem::randomizeFloat(float base, float variation) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    return base + dis(gen) * base * variation;
}

Vector3 ParticleSystem::getRandomDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    Vector3 dir(dis(gen), dis(gen), dis(gen));
    return dir.normalized();
}

Vector3 ParticleSystem::getRandomDirectionInCone(const Vector3& direction, float coneAngle) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    float angle = dis(gen) * coneAngle * M_PI / 180.0f;
    float azimuth = dis(gen) * 2.0f * M_PI;
    
    // Create a random direction within the cone
    Vector3 randomDir = getRandomDirection();
    Vector3 result = direction + randomDir * std::sin(angle);
    
    return result.normalized();
}

ParticleSystem::EffectTemplate& ParticleSystem::getEffectTemplate(EffectType type) {
    return effectTemplates[static_cast<int>(type)];
}

void ParticleSystem::setMaxParticles(int max) {
    maxParticles = std::max(100, max);
    particles.clear();
    particles.resize(maxParticles);
    activeParticleCount = 0;
}