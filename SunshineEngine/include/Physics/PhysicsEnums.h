#pragma once

#include <EASTL/string.h>
#include <nlohmann/json.hpp>

namespace SE {
    // Editor-side enums
    enum class CollisionShape {
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

    enum class PhysicsMotionType {
        Static,
        Kinematic,
        Dynamic
    };

    enum class PhysicsActivation {
        Activate,
        DontActivate
    };

    // JSON enum mappings (serialize as readable strings)
    NLOHMANN_JSON_SERIALIZE_ENUM(CollisionShape, {
        {CollisionShape::Box,                "Box"},
        {CollisionShape::Sphere,             "Sphere"},
        {CollisionShape::Capsule,            "Capsule"},
        {CollisionShape::Cylinder,           "Cylinder"},
        {CollisionShape::TaperedCapsule,     "TaperedCapsule"},
        {CollisionShape::TaperedCylinder,    "TaperedCylinder"},
        {CollisionShape::Plane,              "Plane"},
        {CollisionShape::Triangle,           "Triangle"},
        {CollisionShape::Empty,              "Empty"},
        {CollisionShape::ConvexHull,         "ConvexHull"},
        {CollisionShape::Mesh,               "Mesh"},
        {CollisionShape::HeightField,        "HeightField"},
        {CollisionShape::SoftBody,           "SoftBody"},
        {CollisionShape::StaticCompound,     "StaticCompound"},
        {CollisionShape::MutableCompound,    "MutableCompound"},
        {CollisionShape::Scaled,             "Scaled"},
        {CollisionShape::RotatedTranslated,  "RotatedTranslated"},
        {CollisionShape::OffsetCenterOfMass, "OffsetCenterOfMass"},
        })

        NLOHMANN_JSON_SERIALIZE_ENUM(PhysicsMotionType, {
            {PhysicsMotionType::Static,    "Static"},
            {PhysicsMotionType::Kinematic, "Kinematic"},
            {PhysicsMotionType::Dynamic,   "Dynamic"},
            })

            NLOHMANN_JSON_SERIALIZE_ENUM(PhysicsActivation, {
                {PhysicsActivation::Activate,     "Activate"},
                {PhysicsActivation::DontActivate, "DontActivate"},
                })

}