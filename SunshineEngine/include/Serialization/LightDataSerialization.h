#include <Graphics/Lighting/LightData.h>
#include <Serialization/DXSMSerialization.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE_G {
    // ----------- AmbientLightData -----------
    void to_json(json& j, const AmbientLightData& obj) {
        j = json{
            {"Ambient", obj.Ambient},
            {"AmbientPad", obj.AmbientPad}
        };
    }
    void from_json(const json& j, AmbientLightData& obj) {
        j.at("Ambient").get_to(obj.Ambient);
        j.at("AmbientPad").get_to(obj.AmbientPad);
    }

    // ----------- DirectionalLightData -----------

    void to_json(json& j, const DirectionalLightData& obj) {
        j = json{
            {"Diffuse", obj.Diffuse},
            {"DiffusePad", obj.DiffusePad},
            {"Specular", obj.Specular},
            {"SpecularPad", obj.SpecularPad},
            {"Position", obj.Position},
            {"pad1", obj.pad1},
            {"Direction", obj.Direction},
            {"pad2", obj.pad2}
        };
    }
    void from_json(const json& j, DirectionalLightData& obj) {
        j.at("Diffuse").get_to(obj.Diffuse);
        j.at("DiffusePad").get_to(obj.DiffusePad);
        j.at("Specular").get_to(obj.Specular);
        j.at("SpecularPad").get_to(obj.SpecularPad);
        j.at("Position").get_to(obj.Position);
        j.at("pad1").get_to(obj.pad1);
        j.at("Direction").get_to(obj.Direction);
        j.at("pad2").get_to(obj.pad2);
    }

    // ----------- PointLightData -----------

    void to_json(json& j, const PointLightData& obj) {
        j = json{
            {"Diffuse", obj.Diffuse},
            {"DiffusePad", obj.DiffusePad},
            {"Specular", obj.Specular},
            {"SpecularPad", obj.SpecularPad},
            {"Position", obj.Position},
            {"Range", obj.Range},
            {"Att", obj.Att},
            {"pad", obj.pad}
        };
    }
    void from_json(const json& j, PointLightData& obj) {
        j.at("Diffuse").get_to(obj.Diffuse);
        j.at("DiffusePad").get_to(obj.DiffusePad);
        j.at("Specular").get_to(obj.Specular);
        j.at("SpecularPad").get_to(obj.SpecularPad);
        j.at("Position").get_to(obj.Position);
        j.at("Range").get_to(obj.Range);
        j.at("Att").get_to(obj.Att);
        j.at("pad").get_to(obj.pad);
    }

    // ----------- SpotLightData -----------
    void to_json(json& j, const SpotLightData& obj) {
        j = json{
            {"Diffuse", obj.Diffuse},
            {"DiffusePad", obj.DiffusePad},
            {"Specular", obj.Specular},
            {"SpecularPad", obj.SpecularPad},
            {"Position", obj.Position},
            {"Range", obj.Range},
            {"Direction", obj.Direction},
            {"Spot", obj.Spot},
            {"Att", obj.Att},
            {"pad", obj.pad}
        };
    }
    void from_json(const json& j, SpotLightData& obj) {
        j.at("Diffuse").get_to(obj.Diffuse);
        j.at("DiffusePad").get_to(obj.DiffusePad);
        j.at("Specular").get_to(obj.Specular);
        j.at("SpecularPad").get_to(obj.SpecularPad);
        j.at("Position").get_to(obj.Position);
        j.at("Range").get_to(obj.Range);
        j.at("Direction").get_to(obj.Direction);
        j.at("Spot").get_to(obj.Spot);
        j.at("Att").get_to(obj.Att);
        j.at("pad").get_to(obj.pad);
    }

    // ----------- SkyBoxData -----------
    void to_json(json& j, const SkyBoxData& obj) {
        j = json{
            {"Tint", obj.Tint},
            {"Power", obj.Power}
        };
    }
    void from_json(const json& j, SkyBoxData& obj) {
        j.at("Tint").get_to(obj.Tint);
        j.at("Power").get_to(obj.Power);
    }
}