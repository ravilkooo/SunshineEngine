#pragma once

#include <EASTL/string.h>
#include <nlohmann/json.hpp>

// Editor-side enums
enum class EditorShape {
    // Primitives
    Box,
    Sphere,
    Capsule,
    Cylinder,
    TaperedCapsule,
    TaperedCylinder,
    Plane,
    Triangle,
    Empty,

    // Convex / Mesh
    ConvexHull,
    Mesh,

    // Heightfield
    HeightField,

    // Soft body
    SoftBody,

    // Compound
    StaticCompound,
    MutableCompound,

    // Decorators (wrap a child shape)
    Scaled,
    RotatedTranslated,
    OffsetCenterOfMass
};

enum class EditorMotionType {
    Static,
    Kinematic,
    Dynamic
};

enum class EditorActivation {
    Activate,
    DontActivate
};

// JSON enum mappings (serialize as readable strings)
NLOHMANN_JSON_SERIALIZE_ENUM(EditorShape, {
    {EditorShape::Box,                "Box"},
    {EditorShape::Sphere,             "Sphere"},
    {EditorShape::Capsule,            "Capsule"},
    {EditorShape::Cylinder,           "Cylinder"},
    {EditorShape::TaperedCapsule,     "TaperedCapsule"},
    {EditorShape::TaperedCylinder,    "TaperedCylinder"},
    {EditorShape::Plane,              "Plane"},
    {EditorShape::Triangle,           "Triangle"},
    {EditorShape::Empty,              "Empty"},
    {EditorShape::ConvexHull,         "ConvexHull"},
    {EditorShape::Mesh,               "Mesh"},
    {EditorShape::HeightField,        "HeightField"},
    {EditorShape::SoftBody,           "SoftBody"},
    {EditorShape::StaticCompound,     "StaticCompound"},
    {EditorShape::MutableCompound,    "MutableCompound"},
    {EditorShape::Scaled,             "Scaled"},
    {EditorShape::RotatedTranslated,  "RotatedTranslated"},
    {EditorShape::OffsetCenterOfMass, "OffsetCenterOfMass"},
    })

NLOHMANN_JSON_SERIALIZE_ENUM(EditorMotionType, {
    {EditorMotionType::Static,    "Static"},
    {EditorMotionType::Kinematic, "Kinematic"},
    {EditorMotionType::Dynamic,   "Dynamic"},
    })

NLOHMANN_JSON_SERIALIZE_ENUM(EditorActivation, {
    {EditorActivation::Activate,     "Activate"},
    {EditorActivation::DontActivate, "DontActivate"},
    })
