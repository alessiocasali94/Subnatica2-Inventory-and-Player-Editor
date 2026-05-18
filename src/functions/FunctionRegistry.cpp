#include "functions/FunctionRegistry.h"

namespace functions {

const char* CategoryName(Category c) {
    switch (c) {
    case Category::PlayerStats: return "Player Stats";
    case Category::Movement: return "Movement & Physics";
    case Category::Crafting: return "Crafting & Building";
    case Category::TimeWorld: return "Time & World";
    case Category::Vehicles: return "Vehicles & Tools";
    case Category::Unique: return "Unique / Innovative";
    case Category::Presets: return "Presets & Theme";
    default: return "?";
    }
}

const char* CategoryShortName(Category c) {
    switch (c) {
    case Category::PlayerStats: return "Player";
    case Category::Movement: return "Movement";
    case Category::Crafting: return "Crafting";
    case Category::TimeWorld: return "Time & World";
    case Category::Vehicles: return "Vehicles";
    case Category::Unique: return "Unique";
    case Category::Presets: return "Presets";
    default: return "?";
    }
}

const char* CategoryIcon(Category c) {
    switch (c) {
    case Category::PlayerStats: return "+";
    case Category::Movement: return ">";
    case Category::Crafting: return "#";
    case Category::TimeWorld: return "~";
    case Category::Vehicles: return "V";
    case Category::Unique: return "*";
    case Category::Presets: return "=";
    default: return "-";
    }
}

FunctionRegistry::FunctionRegistry() {
    RegisterDefaults();
}

TrainerFunction& FunctionRegistry::Add(TrainerFunction fn) {
    functions_.push_back(std::move(fn));
    return functions_.back();
}

TrainerFunction* FunctionRegistry::Find(const std::string& id) {
    for (auto& f : functions_)
        if (f.id == id)
            return &f;
    return nullptr;
}

std::vector<TrainerFunction*> FunctionRegistry::ByCategory(Category cat) {
    std::vector<TrainerFunction*> out;
    for (auto& f : functions_)
        if (f.category == cat)
            out.push_back(&f);
    return out;
}

void FunctionRegistry::RegisterDefaults() {
    Add({.id = "health", .label = "Extended Health", .category = Category::PlayerStats,
         .kind = FunctionKind::ToggleSlider, .value = 100.f, .minValue = 0.f, .maxValue = 999.f,
         .hotkeyHint = "Num1"});
    Add({.id = "oxygen", .label = "Extended Oxygen", .category = Category::PlayerStats,
         .kind = FunctionKind::ToggleBar, .value = 1.f, .hotkeyHint = "Num2"});
    Add({.id = "food_water", .label = "Extended Food & Water", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle});
    Add({.id = "energy", .label = "Extended Energy / Batteries", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle});
    Add({.id = "radiation", .label = "Radiation Shield", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle});
    Add({.id = "temperature", .label = "Stable Temperature",
         .description = "Comfort thermoregulation", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle});
    Add({.id = "player_invuln", .label = "Invulnerability", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle, .hotkeyHint = "F1"});
    Add({.id = "stamina", .label = "Extended Stamina", .category = Category::PlayerStats,
         .kind = FunctionKind::Toggle});

    Add({.id = "walk_speed", .label = "Walk Speed Boost", .category = Category::Movement,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 1.f, .maxValue = 10.f});
    Add({.id = "swim_speed", .label = "Swim Speed Boost", .category = Category::Movement,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 1.f, .maxValue = 10.f});
    Add({.id = "jump_height", .label = "Jump Height Boost", .category = Category::Movement,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 1.f, .maxValue = 8.f});
    Add({.id = "aerial_move", .label = "Aerial Movement", .description = "Altitude cap (visual)",
         .category = Category::Movement, .kind = FunctionKind::ToggleSlider, .value = 50.f,
         .minValue = 5.f, .maxValue = 500.f});
    Add({.id = "free_move", .label = "Free Movement", .category = Category::Movement,
         .kind = FunctionKind::Toggle, .hotkeyHint = "F2"});
    Add({.id = "gravity", .label = "Gravity Control", .category = Category::Movement,
         .kind = FunctionKind::Combo, .comboIndex = 1,
         .comboItems = {"Low", "Normal", "Zero", "High"}});
    Add({.id = "teleport", .label = "Waypoint Presets", .category = Category::Movement,
         .kind = FunctionKind::TeleportButtons,
         .teleportLabels = {"Surface", "Deep Abyss", "Lava Zone", "Lost River", "Saved #1"}});

    Add({.id = "instant_craft", .label = "Fast Crafting", .category = Category::Crafting,
         .kind = FunctionKind::Toggle});
    Add({.id = "easy_craft", .label = "Simple Crafting", .category = Category::Crafting,
         .kind = FunctionKind::Toggle});
    Add({.id = "fast_build", .label = "Fast Build Mode", .category = Category::Crafting,
         .kind = FunctionKind::Toggle});
    Add({.id = "base_power", .label = "Extended Base Power", .category = Category::Crafting,
         .kind = FunctionKind::Toggle});
    Add({.id = "unlock_blueprints", .label = "Reveal Blueprints", .category = Category::Crafting,
         .kind = FunctionKind::ButtonProgress});
    Add({.id = "unlock_scans", .label = "Reveal Databank Entries", .category = Category::Crafting,
         .kind = FunctionKind::Button});

    Add({.id = "freeze_time", .label = "Freeze Time of Day", .category = Category::TimeWorld,
         .kind = FunctionKind::ToggleSlider, .value = 12.f, .minValue = 0.f, .maxValue = 24.f});
    Add({.id = "game_speed", .label = "Simulation Speed", .category = Category::TimeWorld,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 0.1f, .maxValue = 5.f});
    Add({.id = "eternal_day", .label = "Day / Night Lock", .category = Category::TimeWorld,
         .kind = FunctionKind::Combo, .comboItems = {"Off", "Day", "Night"}});
    Add({.id = "weather", .label = "Weather Control", .category = Category::TimeWorld,
         .kind = FunctionKind::Combo, .comboItems = {"Calm", "Overcast", "Storm", "Fog"}});
    Add({.id = "slow_ai", .label = "Creature Pace", .category = Category::TimeWorld,
         .kind = FunctionKind::Toggle});

    Add({.id = "vehicle_invuln", .label = "Vehicle Protection",
         .description = "Seamoth, Prawn, Cyclops...", .category = Category::Vehicles,
         .kind = FunctionKind::Toggle});
    Add({.id = "vehicle_energy", .label = "Extended Vehicle Power", .category = Category::Vehicles,
         .kind = FunctionKind::Toggle});
    Add({.id = "vehicle_speed", .label = "Vehicle Speed Boost", .category = Category::Vehicles,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 1.f, .maxValue = 8.f});
    Add({.id = "vehicle_repair", .label = "Vehicle Maintenance", .category = Category::Vehicles,
         .kind = FunctionKind::Button});
    Add({.id = "scanner", .label = "Scanner Range Multiplier", .category = Category::Vehicles,
         .kind = FunctionKind::Slider, .value = 1.f, .minValue = 1.f, .maxValue = 20.f});

    Add({.id = "bio_aura", .label = "Bioluminescence Aura",
         .description = "Visual ambience preset", .category = Category::Unique,
         .kind = FunctionKind::Toggle});
    Add({.id = "creature_whisperer", .label = "Creature Calm",
         .description = "Aggression bias (visual status)", .category = Category::Unique,
         .kind = FunctionKind::ToggleSlider, .value = 0.2f, .minValue = 0.f, .maxValue = 1.f});
    Add({.id = "abyss_vision", .label = "Abyss Vision",
         .description = "Night vision + fog + draw distance", .category = Category::Unique,
         .kind = FunctionKind::ToggleSlider, .value = 0.7f, .minValue = 0.f, .maxValue = 1.f});
    Add({.id = "base_harmony", .label = "Base Harmony",
         .description = "Auto-optimize base integrity & decor", .category = Category::Unique,
         .kind = FunctionKind::Toggle});
    Add({.id = "echo_locator", .label = "Echo Locator", .category = Category::Unique,
         .kind = FunctionKind::Toggle});
    Add({.id = "time_capsule", .label = "Time Capsule Editor", .category = Category::Unique,
         .kind = FunctionKind::TextFields,
         .textFieldLabels = {"Title", "Message", "Screenshot note"},
         .textFieldValues = {"", "", ""}});
    Add({.id = "ocean_mood", .label = "Ocean Mood Changer", .category = Category::Unique,
         .kind = FunctionKind::MoodFilters,
         .comboItems = {"Peaceful", "Horror", "Vibrant", "Melancholy"}});
    Add({.id = "passive_slots", .label = "Passive Mod Slots", .category = Category::Unique,
         .kind = FunctionKind::ModSlotList, .value = 3.f, .minValue = 1.f, .maxValue = 12.f});
    Add({.id = "legacy_explorer", .label = "Legacy Explorer", .category = Category::Unique,
         .kind = FunctionKind::LoreGallery});
}

} // namespace functions
