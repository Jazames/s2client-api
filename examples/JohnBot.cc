#include <sc2api\sc2_api.h>

#include <iostream>

using namespace sc2;

struct IsVespeneGeyser {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
        case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
        case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
        case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
        default: return false;
        }
    }
};

class Bot : public Agent {
public:
    size_t CountUnitType(UNIT_TYPEID unit_type) {
        return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
    }

    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    bool orderUnits(UNIT_TYPEID unit_type, ABILITY_ID ability, Tag target) {
        Units units = Observation()->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            if (unit->unit_type == unit_type) {
                Actions()->UnitCommand(unit, ability, target);
            }
        }
        return true;
    }

    bool TryBuildStructure(ABILITY_ID ability_Type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV) {
        const ObservationInterface* observation = Observation();

        // If a unit already is building a supply structure of this type, do nothing.
        // Also get an scv to build the structure.
        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (order.ability_id == ability_Type_for_structure) {
                   // return false;
                }
            }
            if (unit->unit_type == unit_type) {
                unit_to_build = unit;
            }
        }

        float rx = GetRandomScalar();
        float ry = GetRandomScalar();

        if (unit_to_build != nullptr)
            Actions()->UnitCommand(unit_to_build,
                ability_Type_for_structure,
                Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f));

        return true;
    }

    bool TryBuildSupplyDepot() {
        const ObservationInterface* observation = Observation();

        // If we are not supply capped don't build a supply depot.
        
        if (observation->GetFoodUsed() <= observation->GetFoodCap() - 3)
            return false;
            
        // Try and build a depot. Find a random SCV and give it the order.
        return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT);
    }

    bool TryBuildBarracks() {
        const ObservationInterface* observation = Observation();

        if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) {
            return false;
        }

        if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 4) {
            return false;
        }

        return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS);
    }

    bool TryBuildGas() {
        const ObservationInterface* observation = Observation();
        Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser());


        // If a unit already is building a refinery structure of this type, do nothing.
        // Also get an scv to build the structure.
        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            if (unit->unit_type == UNIT_TYPEID::TERRAN_SCV) {
                unit_to_build = unit;
                break;
            }
        }
        Tag closestGeyser = 0;
        float minimum_distance = 30.0f;
        for (const auto& geyser : geysers) {
            float current_distance = Distance2D(unit_to_build->pos, geyser->pos);
            if (current_distance < minimum_distance) {
                minimum_distance = current_distance;
                closestGeyser = geyser->tag;
            }
        }

        if (closestGeyser == 0) {
           // std::cout << "failded" << std::endl;
            return false;
        }

        Actions()->UnitCommand(unit_to_build,
            ABILITY_ID::BUILD_REFINERY,
            observation->GetUnit(closestGeyser) );

        return true;
    }

    const Unit* FindNearestMineralPatch(const Point2D& start) {
        Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
        float distance = std::numeric_limits<float>::max();
        const Unit* target = nullptr;
        for (const auto& u : units) {
            if (u->unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
                float d = DistanceSquared2D(u->pos, start);
                if (d < distance) {
                    distance = d;
                    target = u;
                }
            }
        }
        return target;
    }

    virtual void OnStep() {
        TryBuildSupplyDepot();

        TryBuildBarracks();

       // TryBuildGas();
       // std::cout << Observation()->GetGameLoop() << std::endl;
    }

    virtual void OnUnitIdle(const Unit* unit) final {
        switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::TERRAN_COMMANDCENTER: {
            if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < 20)
                Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
            break;
        }
        case UNIT_TYPEID::TERRAN_SCV: {
            const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
            if (!mineral_target) {
                break;
            }
            Actions()->UnitCommand(unit, ABILITY_ID::SMART,mineral_target);
            break;
        }
        case UNIT_TYPEID::TERRAN_BARRACKS: {
            Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
            break;
        }
        case UNIT_TYPEID::TERRAN_MARINE: {
            const GameInfo& game_info = Observation()->GetGameInfo();
            if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 20) {
                Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
            }
            break;
        }
        default: {
            break;
        }
        }
    }
};

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &bot),
        CreateComputer(Race::Zerg,sc2::Difficulty::Medium)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()){
        
    }

    return 0;
}
