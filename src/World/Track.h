#pragma once
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include <vector>

class Track {
public:
    struct TrackPoint {
        Vector3 position;
        Vector3 tangent;
        Vector3 normal;
        Vector3 binormal;
        float width;
        float banking;
        float curvature;
    };

    struct Checkpoint {
        Vector3 position;
        Vector3 direction;
        float width;
        float height;
        int lapNumber;
        bool isStartFinish;
    };

private:
    std::vector<TrackPoint> trackPoints;
    std::vector<Checkpoint> checkpoints;
    std::vector<Vector3> trackVertices;
    std::vector<Vector3> trackNormals;
    std::vector<Vector3> trackUVs;
    std::vector<unsigned int> trackIndices;
    
    float trackLength;
    float trackWidth;
    int numLaps;
    int currentLap;
    int currentCheckpoint;
    
    // Track generation parameters
    int trackResolution;
    float trackRadius;
    float trackComplexity;
    float bankingAmount;
    
    // Visual properties
    Vector3 trackColor;
    Vector3 trackLineColor;
    float trackLineWidth;
    
public:
    Track();
    Track(float radius, float width, int resolution);
    
    // Track generation
    void generateCircularTrack(float radius, float width, int resolution);
    void generateFigureEightTrack(float radius, float width, int resolution);
    void generateCustomTrack(const std::vector<Vector3>& controlPoints, float width, int resolution);
    void generateRandomTrack(float radius, float width, int resolution, float complexity);
    
    // Track queries
    TrackPoint getTrackPoint(float t) const;
    TrackPoint getTrackPointAtDistance(float distance) const;
    Vector3 getTrackPosition(float t) const;
    Vector3 getTrackTangent(float t) const;
    Vector3 getTrackNormal(float t) const;
    Vector3 getTrackBinormal(float t) const;
    float getTrackWidth(float t) const;
    float getTrackBanking(float t) const;
    float getTrackCurvature(float t) const;
    
    // Distance and parameter conversion
    float getTrackLength() const { return trackLength; }
    float getDistanceAlongTrack(const Vector3& position) const;
    float getParameterFromDistance(float distance) const;
    float getParameterFromPosition(const Vector3& position) const;
    
    // Checkpoint system
    void generateCheckpoints(int numCheckpoints);
    bool checkCheckpoint(const Vector3& position, float radius);
    int getCurrentLap() const { return currentLap; }
    int getCurrentCheckpoint() const { return currentCheckpoint; }
    void resetLapProgress();
    
    // Track geometry
    void generateTrackGeometry();
    const std::vector<Vector3>& getTrackVertices() const { return trackVertices; }
    const std::vector<Vector3>& getTrackNormals() const { return trackNormals; }
    const std::vector<Vector3>& getTrackUVs() const { return trackUVs; }
    const std::vector<unsigned int>& getTrackIndices() const { return trackIndices; }
    
    // Track properties
    void setTrackWidth(float width);
    void setBankingAmount(float banking);
    void setTrackColor(const Vector3& color);
    void setTrackLineColor(const Vector3& color);
    void setTrackLineWidth(float width);
    
    // Utility functions
    Vector3 getClosestPointOnTrack(const Vector3& position) const;
    float getDistanceToTrack(const Vector3& position) const;
    bool isPositionOnTrack(const Vector3& position, float tolerance = 1.0f) const;
    
    // Debug
    void debugDraw() const;
    
private:
    void calculateTrackProperties();
    void calculateTrackNormals();
    void calculateTrackBanking();
    void calculateTrackCurvature();
    Vector3 interpolatePosition(float t) const;
    Vector3 interpolateTangent(float t) const;
    Vector3 interpolateNormal(float t) const;
    Vector3 interpolateBinormal(float t) const;
    float interpolateWidth(float t) const;
    float interpolateBanking(float t) const;
    float interpolateCurvature(float t) const;
};