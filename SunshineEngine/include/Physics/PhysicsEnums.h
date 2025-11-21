#pragma once

#include <EASTL/string.h>
#include <nlohmann/json.hpp>

namespace SE {
    // Editor-side enums
    enum class ColliderShapeType {
        // Primitives
        Sphere,
        Box,
        Capsule,
        TaperedCapsule,
        Cylinder,
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
    NLOHMANN_JSON_SERIALIZE_ENUM(ColliderShapeType, {
        {ColliderShapeType::Box,                "Box"},
        {ColliderShapeType::Sphere,             "Sphere"},
        {ColliderShapeType::Capsule,            "Capsule"},
        {ColliderShapeType::Cylinder,           "Cylinder"},
        {ColliderShapeType::TaperedCapsule,     "TaperedCapsule"},
        {ColliderShapeType::TaperedCylinder,    "TaperedCylinder"},
        {ColliderShapeType::Plane,              "Plane"},
        {ColliderShapeType::Triangle,           "Triangle"},
        {ColliderShapeType::Empty,              "Empty"},
        {ColliderShapeType::ConvexHull,         "ConvexHull"},
        {ColliderShapeType::Mesh,               "Mesh"},
        {ColliderShapeType::HeightField,        "HeightField"},
        {ColliderShapeType::SoftBody,           "SoftBody"},
        {ColliderShapeType::StaticCompound,     "StaticCompound"},
        {ColliderShapeType::MutableCompound,    "MutableCompound"},
        {ColliderShapeType::Scaled,             "Scaled"},
        {ColliderShapeType::RotatedTranslated,  "RotatedTranslated"},
        {ColliderShapeType::OffsetCenterOfMass, "OffsetCenterOfMass"},
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