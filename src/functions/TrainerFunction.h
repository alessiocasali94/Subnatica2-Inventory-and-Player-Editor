#pragma once

#include <string>
#include <vector>

namespace functions {

enum class FunctionKind {
    Toggle,
    Slider,
    ToggleSlider,
    ToggleBar,
    Button,
    ButtonProgress,
    Combo,
    TeleportButtons,
    TextFields,
    MoodFilters,
    ModSlotList,
    LoreGallery,
};

enum class Category {
    PlayerStats,
    Movement,
    Crafting,
    TimeWorld,
    Vehicles,
    Unique,
    Presets,
    Count
};

struct TrainerFunction {
    std::string id;
    std::string label;
    std::string description;
    Category category = Category::PlayerStats;
    FunctionKind kind = FunctionKind::Toggle;

    bool enabled = false;
    float value = 0.f;
    float minValue = 0.f;
    float maxValue = 1.f;
    float anim = 0.f; // 0..1 activation glow

    int comboIndex = 0;
    std::vector<std::string> comboItems;

    std::vector<std::string> teleportLabels;
    std::vector<std::string> textFieldLabels;
    std::vector<std::string> textFieldValues;

    float fakeProgress = 0.f;
    std::string hotkeyHint;

    bool HasSlider() const {
        return kind == FunctionKind::Slider || kind == FunctionKind::ToggleSlider;
    }
};

} // namespace functions
