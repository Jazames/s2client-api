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
            for (const auto soldier : army) {
                if (game_loop % 5 == 0)
                    AttackWithUnit(soldier, observation);
            }  
        }

        return true;
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
        const Unit* larva;
        int optimal_drone = std::min((int)CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) * 16, 48);
        if (GetRandomUnit(larva, observation, UNIT_TYPEID::ZERG_LARVA)) {
            if (observation->GetFoodUsed() > observation->GetFoodCap() - 6) {
                TrainUnit(ABILITY_ID::TRAIN_OVERLORD, larva, observation);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_DRONE) < 32) {
                Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_DRONE);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1) {
                Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_ZERGLING);
            }
            return true;
        }

        return false;
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


