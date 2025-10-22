#include "Track.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Track::Track() 
    : trackLength(0.0f)
    , trackWidth(10.0f)
    , numLaps(3)
    , currentLap(0)
    , currentCheckpoint(0)
    , trackResolution(100)
    , trackRadius(50.0f)
    , trackComplexity(0.5f)
    , bankingAmount(0.0f)
    , trackColor(0.3f, 0.3f, 0.3f)
    , trackLineColor(1.0f, 1.0f, 1.0f)
    , trackLineWidth(0.1f) {
    generateCircularTrack(trackRadius, trackWidth, trackResolution);
}

Track::Track(float radius, float width, int resolution)
    : trackLength(0.0f)
    , trackWidth(width)
    , numLaps(3)
    , currentLap(0)
    , currentCheckpoint(0)
    , trackResolution(resolution)
    , trackRadius(radius)
    , trackComplexity(0.5f)
    , bankingAmount(0.0f)
    , trackColor(0.3f, 0.3f, 0.3f)
    , trackLineColor(1.0f, 1.0f, 1.0f)
    , trackLineWidth(0.1f) {
    generateCircularTrack(radius, width, resolution);
}

void Track::generateCircularTrack(float radius, float width, int resolution) {
    trackPoints.clear();
    trackWidth = width;
    trackResolution = resolution;
    
    for (int i = 0; i < resolution; i++) {
        float t = (float)i / (float)resolution;
        float angle = t * 2.0f * M_PI;
        
        TrackPoint point;
        point.position = Vector3(
            std::cos(angle) * radius,
            0.0f,
            std::sin(angle) * radius
        );
        point.tangent = Vector3(
            -std::sin(angle),
            0.0f,
            std::cos(angle)
        );
        point.normal = Vector3(
            std::cos(angle),
            0.0f,
            std::sin(angle)
        );
        point.binormal = Vector3(0.0f, 1.0f, 0.0f);
        point.width = width;
        point.banking = 0.0f;
        point.curvature = 1.0f / radius;
        
        trackPoints.push_back(point);
    }
    
    calculateTrackProperties();
    generateCheckpoints(8);
    generateTrackGeometry();
}

void Track::generateFigureEightTrack(float radius, float width, int resolution) {
    trackPoints.clear();
    trackWidth = width;
    trackResolution = resolution;
    
    for (int i = 0; i < resolution; i++) {
        float t = (float)i / (float)resolution;
        float angle = t * 4.0f * M_PI; // Two full circles
        
        TrackPoint point;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius * std::sin(angle * 0.5f);
        
        point.position = Vector3(x, 0.0f, z);
        
        // Calculate tangent
        float nextAngle = (t + 1.0f / resolution) * 4.0f * M_PI;
        float nextX = std::cos(nextAngle) * radius;
        float nextZ = std::sin(nextAngle) * radius * std::sin(nextAngle * 0.5f);
        
        Vector3 tangent = Vector3(nextX - x, 0.0f, nextZ - z).normalized();
        point.tangent = tangent;
        
        // Calculate normal and binormal
        point.normal = Vector3(-tangent.z, 0.0f, tangent.x);
        point.binormal = Vector3(0.0f, 1.0f, 0.0f);
        point.width = width;
        point.banking = 0.0f;
        point.curvature = 0.0f; // Will be calculated later
        
        trackPoints.push_back(point);
    }
    
    calculateTrackProperties();
    generateCheckpoints(8);
    generateTrackGeometry();
}

void Track::generateCustomTrack(const std::vector<Vector3>& controlPoints, float width, int resolution) {
    trackPoints.clear();
    trackWidth = width;
    trackResolution = resolution;
    
    if (controlPoints.size() < 2) return;
    
    for (int i = 0; i < resolution; i++) {
        float t = (float)i / (float)resolution;
        
        TrackPoint point;
        point.position = interpolatePosition(t);
        point.tangent = interpolateTangent(t);
        point.normal = interpolateNormal(t);
        point.binormal = interpolateBinormal(t);
        point.width = width;
        point.banking = 0.0f;
        point.curvature = 0.0f;
        
        trackPoints.push_back(point);
    }
    
    calculateTrackProperties();
    generateCheckpoints(8);
    generateTrackGeometry();
}

void Track::generateRandomTrack(float radius, float width, int resolution, float complexity) {
    trackPoints.clear();
    trackWidth = width;
    trackResolution = resolution;
    trackComplexity = complexity;
    
    std::vector<Vector3> controlPoints;
    int numControlPoints = 8;
    
    for (int i = 0; i < numControlPoints; i++) {
        float angle = (float)i / (float)numControlPoints * 2.0f * M_PI;
        float randomRadius = radius * (0.8f + 0.4f * ((float)rand() / RAND_MAX));
        float randomHeight = 2.0f * ((float)rand() / RAND_MAX - 0.5f);
        
        Vector3 point(
            std::cos(angle) * randomRadius,
            randomHeight,
            std::sin(angle) * randomRadius
        );
        controlPoints.push_back(point);
    }
    
    generateCustomTrack(controlPoints, width, resolution);
}

Track::TrackPoint Track::getTrackPoint(float t) const {
    if (trackPoints.empty()) {
        TrackPoint empty;
        return empty;
    }
    
    t = std::fmod(t, 1.0f);
    if (t < 0.0f) t += 1.0f;
    
    float index = t * (trackPoints.size() - 1);
    int i = (int)index;
    float fraction = index - i;
    
    if (i >= trackPoints.size() - 1) {
        return trackPoints.back();
    }
    
    TrackPoint result;
    result.position = trackPoints[i].position + (trackPoints[i + 1].position - trackPoints[i].position) * fraction;
    result.tangent = trackPoints[i].tangent + (trackPoints[i + 1].tangent - trackPoints[i].tangent) * fraction;
    result.normal = trackPoints[i].normal + (trackPoints[i + 1].normal - trackPoints[i].normal) * fraction;
    result.binormal = trackPoints[i].binormal + (trackPoints[i + 1].binormal - trackPoints[i].binormal) * fraction;
    result.width = trackPoints[i].width + (trackPoints[i + 1].width - trackPoints[i].width) * fraction;
    result.banking = trackPoints[i].banking + (trackPoints[i + 1].banking - trackPoints[i].banking) * fraction;
    result.curvature = trackPoints[i].curvature + (trackPoints[i + 1].curvature - trackPoints[i].curvature) * fraction;
    
    return result;
}

Track::TrackPoint Track::getTrackPointAtDistance(float distance) const {
    float t = getParameterFromDistance(distance);
    return getTrackPoint(t);
}

Vector3 Track::getTrackPosition(float t) const {
    return getTrackPoint(t).position;
}

Vector3 Track::getTrackTangent(float t) const {
    return getTrackPoint(t).tangent;
}

Vector3 Track::getTrackNormal(float t) const {
    return getTrackPoint(t).normal;
}

Vector3 Track::getTrackBinormal(float t) const {
    return getTrackPoint(t).binormal;
}

float Track::getTrackWidth(float t) const {
    return getTrackPoint(t).width;
}

float Track::getTrackBanking(float t) const {
    return getTrackPoint(t).banking;
}

float Track::getTrackCurvature(float t) const {
    return getTrackPoint(t).curvature;
}

float Track::getDistanceAlongTrack(const Vector3& position) const {
    float minDistance = std::numeric_limits<float>::max();
    float bestT = 0.0f;
    
    for (int i = 0; i < trackPoints.size(); i++) {
        float t = (float)i / (float)(trackPoints.size() - 1);
        Vector3 trackPos = getTrackPosition(t);
        float distance = (position - trackPos).length();
        
        if (distance < minDistance) {
            minDistance = distance;
            bestT = t;
        }
    }
    
    return bestT * trackLength;
}

float Track::getParameterFromDistance(float distance) const {
    if (trackLength <= 0.0f) return 0.0f;
    return std::fmod(distance / trackLength, 1.0f);
}

float Track::getParameterFromPosition(const Vector3& position) const {
    return getParameterFromDistance(getDistanceAlongTrack(position));
}

void Track::generateCheckpoints(int numCheckpoints) {
    checkpoints.clear();
    
    for (int i = 0; i < numCheckpoints; i++) {
        float t = (float)i / (float)numCheckpoints;
        TrackPoint point = getTrackPoint(t);
        
        Checkpoint checkpoint;
        checkpoint.position = point.position;
        checkpoint.direction = point.tangent;
        checkpoint.width = point.width;
        checkpoint.height = 3.0f;
        checkpoint.lapNumber = 0;
        checkpoint.isStartFinish = (i == 0);
        
        checkpoints.push_back(checkpoint);
    }
}

bool Track::checkCheckpoint(const Vector3& position, float radius) {
    if (checkpoints.empty()) return false;
    
    for (size_t i = 0; i < checkpoints.size(); i++) {
        float distance = (position - checkpoints[i].position).length();
        if (distance < radius) {
            if (i == currentCheckpoint) {
                currentCheckpoint++;
                if (currentCheckpoint >= checkpoints.size()) {
                    currentCheckpoint = 0;
                    currentLap++;
                }
                return true;
            }
        }
    }
    
    return false;
}

void Track::resetLapProgress() {
    currentLap = 0;
    currentCheckpoint = 0;
}

void Track::generateTrackGeometry() {
    trackVertices.clear();
    trackNormals.clear();
    trackUVs.clear();
    trackIndices.clear();
    
    if (trackPoints.empty()) return;
    
    int segments = trackPoints.size();
    int verticesPerSegment = 4; // Left, right, left, right (for quads)
    
    for (int i = 0; i < segments; i++) {
        TrackPoint point = trackPoints[i];
        float t = (float)i / (float)(segments - 1);
        
        Vector3 left = point.position + point.normal * (point.width * 0.5f);
        Vector3 right = point.position - point.normal * (point.width * 0.5f);
        
        trackVertices.push_back(left);
        trackVertices.push_back(right);
        trackNormals.push_back(point.binormal);
        trackNormals.push_back(point.binormal);
        trackUVs.push_back(Vector3(t, 0.0f, 0.0f));
        trackUVs.push_back(Vector3(t, 1.0f, 0.0f));
    }
    
    // Generate indices for quads
    for (int i = 0; i < segments - 1; i++) {
        int base = i * 2;
        int next = (i + 1) * 2;
        
        // First triangle
        trackIndices.push_back(base);
        trackIndices.push_back(next);
        trackIndices.push_back(base + 1);
        
        // Second triangle
        trackIndices.push_back(base + 1);
        trackIndices.push_back(next);
        trackIndices.push_back(next + 1);
    }
}

void Track::setTrackWidth(float width) {
    trackWidth = std::max(0.1f, width);
    generateTrackGeometry();
}

void Track::setBankingAmount(float banking) {
    bankingAmount = banking;
    calculateTrackBanking();
    generateTrackGeometry();
}

void Track::setTrackColor(const Vector3& color) {
    trackColor = color;
}

void Track::setTrackLineColor(const Vector3& color) {
    trackLineColor = color;
}

void Track::setTrackLineWidth(float width) {
    trackLineWidth = std::max(0.01f, width);
}

Vector3 Track::getClosestPointOnTrack(const Vector3& position) const {
    float minDistance = std::numeric_limits<float>::max();
    Vector3 closestPoint;
    
    for (const TrackPoint& point : trackPoints) {
        float distance = (position - point.position).length();
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = point.position;
        }
    }
    
    return closestPoint;
}

float Track::getDistanceToTrack(const Vector3& position) const {
    return (position - getClosestPointOnTrack(position)).length();
}

bool Track::isPositionOnTrack(const Vector3& position, float tolerance) const {
    return getDistanceToTrack(position) <= tolerance;
}

void Track::debugDraw() const {
    // Debug visualization would be implemented here
    // This would typically render:
    // - Track centerline
    // - Track boundaries
    // - Checkpoint positions
    // - Start/finish line
    // - Track segments
    // - Banking angles
    
    // For now, we'll just print debug info to console in debug builds
    #ifdef DEBUG_BUILD
    std::cout << "Track Debug Info:" << std::endl;
    std::cout << "  Type: " << static_cast<int>(trackType) << std::endl;
    std::cout << "  Radius: " << radius << std::endl;
    std::cout << "  Width: " << width << std::endl;
    std::cout << "  Resolution: " << resolution << std::endl;
    std::cout << "  Banking: " << banking << std::endl;
    std::cout << "  Vertices: " << vertices.size() << std::endl;
    std::cout << "  Indices: " << indices.size() << std::endl;
    std::cout << "  Checkpoints: " << checkpoints.size() << std::endl;
    std::cout << "  Current Lap: " << currentLap << std::endl;
    std::cout << "  Current Checkpoint: " << currentCheckpoint << std::endl;
    #endif
}

void Track::calculateTrackProperties() {
    if (trackPoints.empty()) {
        trackLength = 0.0f;
        return;
    }
    
    trackLength = 0.0f;
    for (size_t i = 1; i < trackPoints.size(); i++) {
        float segmentLength = (trackPoints[i].position - trackPoints[i-1].position).length();
        trackLength += segmentLength;
    }
}

void Track::calculateTrackNormals() {
    for (size_t i = 0; i < trackPoints.size(); i++) {
        Vector3 tangent = trackPoints[i].tangent;
        trackPoints[i].normal = Vector3(-tangent.z, 0.0f, tangent.x);
        trackPoints[i].binormal = tangent.cross(trackPoints[i].normal);
    }
}

void Track::calculateTrackBanking() {
    for (size_t i = 0; i < trackPoints.size(); i++) {
        float t = (float)i / (float)(trackPoints.size() - 1);
        trackPoints[i].banking = std::sin(t * 2.0f * M_PI) * bankingAmount;
    }
}

void Track::calculateTrackCurvature() {
    for (size_t i = 0; i < trackPoints.size(); i++) {
        int prev = (i - 1 + trackPoints.size()) % trackPoints.size();
        int next = (i + 1) % trackPoints.size();
        
        Vector3 p1 = trackPoints[prev].position;
        Vector3 p2 = trackPoints[i].position;
        Vector3 p3 = trackPoints[next].position;
        
        Vector3 v1 = p2 - p1;
        Vector3 v2 = p3 - p2;
        
        float curvature = v1.cross(v2).length() / (v1.length() * v2.length() * v2.length());
        trackPoints[i].curvature = curvature;
    }
}

Vector3 Track::interpolatePosition(float t) const {
    // This would implement proper spline interpolation
    // For now, return a simple circular position
    float angle = t * 2.0f * M_PI;
    return Vector3(std::cos(angle) * trackRadius, 0.0f, std::sin(angle) * trackRadius);
}

Vector3 Track::interpolateTangent(float t) const {
    // This would implement proper spline tangent calculation
    float angle = t * 2.0f * M_PI;
    return Vector3(-std::sin(angle), 0.0f, std::cos(angle));
}

Vector3 Track::interpolateNormal(float t) const {
    // This would implement proper spline normal calculation
    float angle = t * 2.0f * M_PI;
    return Vector3(std::cos(angle), 0.0f, std::sin(angle));
}

Vector3 Track::interpolateBinormal(float t) const {
    return Vector3(0.0f, 1.0f, 0.0f);
}

float Track::interpolateWidth(float t) const {
    return trackWidth;
}

float Track::interpolateBanking(float t) const {
    return 0.0f;
}

float Track::interpolateCurvature(float t) const {
    return 1.0f / trackRadius;
}