#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>

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

class JohnBot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "ZingRush1" << std::endl;
    }

    virtual void OnStep() {
        const ObservationInterface* observation = Observation();
        uint32_t game_loop = Observation()->GetGameLoop();
        ManageLarva(observation);
        ManageStructures(observation);
        ManageArmy(observation);
        if (game_loop % 30 == 0) {
            ManageMining(observation);
        }
        
        // std::cout << Observation()->GetGameLoop() << std::endl;
    }

    virtual void OnUnitIdle(const Unit* unit) final {
        switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::ZERG_DRONE: {
            const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
            if (!mineral_target) {
                break;
            }
            Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
            break;
        }
        case UNIT_TYPEID::ZERG_HATCHERY: {
            if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1) {
               // Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_QUEEN);
            }
            break;
        }
        case UNIT_TYPEID::ZERG_ZERGLING: {
            break;
        }
        default: {
            break;
        }
        }
    }


private:
    size_t CountUnitType(UNIT_TYPEID unit_type) {
        return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
    }

    bool GetRandomUnit(const Unit*& unit_out, const ObservationInterface* observation, UnitTypeID unit_type) {
        Units my_units = observation->GetUnits(Unit::Alliance::Self);
        std::random_shuffle(my_units.begin(), my_units.end());
        for (const auto unit : my_units) {
            if (unit->unit_type == unit_type) {
                unit_out = unit;
                return true;
            }
        }
        return false;
    }


    bool ManageArmy(const ObservationInterface* observation) {
        uint32_t game_loop = Observation()->GetGameLoop();
        if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) > 20) {
            Units army = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_ZERGLING));
            static Point2D rally = army.front()->pos;
            int armycount = 0;
            for (const auto soldier : army) {
                if (game_loop % 5 == 0) {
                    // group up if not in one group
                    if (CountNeibors(soldier, UNIT_TYPEID::ZERG_ZERGLING, 10.0f, observation) >= 15) {
                        AttackWithUnit(soldier, observation);
                    }
                    else {
                        Actions()->UnitCommand(soldier, ABILITY_ID::ATTACK_ATTACK, rally);
                    }
                }
            }  
        }

        return true;
    }

    size_t CountNeibors(const Unit* unit, UNIT_TYPEID unit_type, float distance, const ObservationInterface* observation) {
        size_t neibors = 0;
        Units freinds = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));

        for (const auto& freind : freinds) {
            if (Distance2D(freind->pos, unit->pos) <= distance) {
                neibors++;
            }
        }
        return neibors;
    }

    bool AttackWithUnit(const Unit* unit, const ObservationInterface* observation) {
        Units enemy_unit = observation->GetUnits(Unit::Alliance::Enemy);
        if (enemy_unit.empty()) {
            GameInfo game_info = observation->GetGameInfo();
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, game_info.enemy_start_locations.front());
        }
        else {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, enemy_unit.front()->pos);
        }
        return true;
    }

    bool ManageLarva(const ObservationInterface* observation) {
        size_t drone_count = 16;
        size_t overlord_count = 1;
        size_t ling_count = 0;
        if (observation->GetFoodUsed() > observation->GetFoodCap() - 4) {
            overlord_count = (observation->GetFoodCap() / 6) + 1;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1) {
            ling_count = 200;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) > 20) {
            drone_count = CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) / 2;
        }
        
        TryTrainUnits(UNIT_TYPEID::ZERG_DRONE, ABILITY_ID::TRAIN_DRONE, drone_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_OVERLORD, ABILITY_ID::TRAIN_OVERLORD, overlord_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_ZERGLING, ABILITY_ID::TRAIN_ZERGLING, ling_count, observation);

        return true;
    }

    bool TryTrainUnits(UNIT_TYPEID unit_type, ABILITY_ID ability_type, size_t number_of_units, const ObservationInterface* observation) {
        size_t number_existing_units = CountUnitType(unit_type);
        Units eggs = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_EGG));
        for (const auto& egg : eggs) {
            if (!egg->orders.empty()) {
                if (egg->orders.front().ability_id == ability_type) {
                    number_existing_units++;
                }
            }
        }

        // if more units are needed spawn them
        if (number_existing_units < number_of_units) {
            const Unit* larva;
            if (GetRandomUnit(larva, observation, UNIT_TYPEID::ZERG_LARVA)) {
                Actions()->UnitCommand(larva, ability_type);
                return true;
            }
            return false;
        }
        return true;
    }

    bool ManageStructures(const ObservationInterface* observation) {
        if (CountUnitType(UNIT_TYPEID::ZERG_DRONE) >= 16 && CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
            TryBuildStructure(ABILITY_ID::BUILD_SPAWNINGPOOL);
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_LARVA) <= 1 && observation->GetMinerals() >= 300 && CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) < 8) {
            TryBuildExpantion();
        }
        return true;
    }

    bool TrainUnit(AbilityID ability_type_for_unit, const Unit* unitToTrain, const ObservationInterface* observation) {
        // if an overlord is spawning do nothing
        Units eggs = observation->GetUnits(Unit::Self, IsUnit(UNIT_TYPEID::ZERG_EGG));
        for (const auto egg : eggs) {
            if (!egg->orders.empty()) {
                if (egg->orders.front().ability_id == ability_type_for_unit) {
                    return false;
                }
            }
        }
        Actions()->UnitCommand(unitToTrain, ability_type_for_unit);
        return true;
    }

    bool TryBuildStructure(ABILITY_ID ability_Type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::ZERG_DRONE) {
        const ObservationInterface* observation = Observation();

        // If a unit already is building a supply structure of this type, do nothing.
        // Also get an scv to build the structure.
        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (order.ability_id == ability_Type_for_structure) {
                    return false;
                }
            }
            if (unit->unit_type == unit_type) {
                unit_to_build = unit;
            }
        }

        float rx = GetRandomScalar();
        float ry = GetRandomScalar();
        Point2D buildPoint(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f);


        if (unit_to_build != nullptr && observation->HasCreep(buildPoint))
            Actions()->UnitCommand(unit_to_build,
                ability_Type_for_structure,
                buildPoint);

        return true;
    }

    bool NearestDrone(const Unit*& unit_out,const Unit* hive, const ObservationInterface* observation) {
        Units drones = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_DRONE));
        const Unit* closest_drone = nullptr;
        float distance = std::numeric_limits<float>::max();
        for (const auto& drone : drones) {
            float check_distance = Distance2D(hive->pos, drone->pos);
            if (check_distance <= distance) {
                closest_drone = drone;
                distance = check_distance;
            }
        }
        if (closest_drone == nullptr)
            return false;
        else {
            unit_out = closest_drone;
            return true;
        }
    }

    bool ManageMining(const ObservationInterface* observation) {
        Units townHalls = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_HATCHERY));

        for (const auto& hive : townHalls) {
            // if has the ideal number of harvesters do nothing
            if (hive->ideal_harvesters == 0) {
                continue;
            }

            // if has too many harvesters, move a drone to annother base.
            if (hive->assigned_harvesters > hive->ideal_harvesters) {
                const Unit* drone = nullptr;
                if (NearestDrone(drone, hive, observation)) {
                    // send drone to next hive
                    const Unit* other_hive = nullptr;
                    if (GetRandomUnit(other_hive, observation, UNIT_TYPEID::ZERG_HATCHERY)) {
                        Actions()->UnitCommand(drone, ABILITY_ID::SMART, other_hive->pos);
                    }
                }
            }
        }

        return true;
    }

    bool TryBuildExpantion() {
        const ObservationInterface* observation = Observation();

        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self);
        for (const auto& unit : units) {
            for (const auto& order : unit->orders) {
                if (order.ability_id == ABILITY_ID::BUILD_HATCHERY) {
                    return false;
                }
            }
            if (unit->unit_type == UNIT_TYPEID::ZERG_DRONE) {
                unit_to_build = unit;
            }
        }

        auto start_location = observation->GetStartLocation();
        Actions()->UnitCommand(unit_to_build, ABILITY_ID::BUILD_HATCHERY, NearestExpantion(start_location, observation));
    }

    Point3D NearestExpantion(Point2D start, const ObservationInterface* observation) {
        float min_distance = std::numeric_limits<float>::max();
        Point3D closest_expantion;
        auto expansions = search::CalculateExpansionLocations(observation, Query());
        for (const auto& expansion : expansions) {
            float curr_distance = Distance2D(start, expansion);
            if (curr_distance < 0.01f) {
                continue;
            }
            if (curr_distance < min_distance) {
                min_distance = curr_distance;
                closest_expantion = expansion;
            }
        }

        return closest_expantion;
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

   
};


