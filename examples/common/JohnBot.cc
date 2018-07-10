//{C++}
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

struct IsTownHall {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
        case UNIT_TYPEID::ZERG_HATCHERY: return true;
        case UNIT_TYPEID::ZERG_LAIR: return true;
        case UNIT_TYPEID::ZERG_HIVE: return true;
        default: return false;
        }
    }
};

class JohnBot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "ZingRush4" << std::endl;
    }

    virtual void OnStep() {
        const ObservationInterface* observation = Observation();
        uint32_t game_loop = Observation()->GetGameLoop();
        TrainQueens(observation);
        ManageLarva(observation);
        ManageStructures(observation);
        ManageQueens(observation);
        ManageArmy(observation);
        if (game_loop % 30 == 0) {
            ManageMining(observation);
        }

// std::cout << Observation()->GetGameLoop() << std::endl;
    }

    virtual void OnUnitIdle(const Unit* unit) final {
        const ObservationInterface* observation = Observation();
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
              //  Actions()->UnitCommand(unit, ABILITY_ID::MORPH_LAIR);
            }
            break;
        }
        case UNIT_TYPEID::ZERG_SPAWNINGPOOL: {
           
                Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST);
           
            break;
        }
        case UNIT_TYPEID::ZERG_BANELINGNEST: {
            if (CountUnitType(UNIT_TYPEID::ZERG_LAIR)) {
                Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS);
            }
            break;
        }
        case UNIT_TYPEID::ZERG_HYDRALISKDEN: {
            Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_MUSCULARAUGMENTS);
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
            Units army = observation->GetUnits(Unit::Alliance::Self);
            int armycount = 0;
            for (const auto& soldier : army) {
                const float GROUP_DISTANCE = 10.0f;
                switch (soldier->unit_type.ToType())
                {
                case UNIT_TYPEID::ZERG_HYDRALISK: {
                    defend(soldier, observation);
                    if (CountNeibors(soldier, UNIT_TYPEID::ZERG_HYDRALISK, GROUP_DISTANCE + 5.0f, observation) >= 8) {
                        AttackWithUnit(soldier, observation);
                    }
                    else {
                        GroupUp(soldier, army, GROUP_DISTANCE, UNIT_TYPEID::ZERG_HYDRALISK);
                    }
                    break;
                }
                case UNIT_TYPEID::ZERG_BANELING: {
                    defend(soldier, observation);
                    // group up if not in one group
                    if (CountNeibors(soldier, UNIT_TYPEID::ZERG_BANELING, GROUP_DISTANCE + 5.0f, observation) >= 8 ) {
                        if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) < 1 || CountUnitType(UNIT_TYPEID::ZERG_HYDRALISK) >= 8) {
                            AttackWithUnit(soldier, observation);
                        }
                    }
                    else {
                        GroupUp(soldier, army, GROUP_DISTANCE,UNIT_TYPEID::ZERG_BANELING);
                    }
                    break;
                }
                case UNIT_TYPEID::ZERG_ZERGLING: {
                    defend(soldier, observation);
                    if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) < 1 || CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) >= 20) {
                        if (CountNeibors(soldier, UNIT_TYPEID::ZERG_ZERGLING, GROUP_DISTANCE + 5.0f, observation) >= 20) {
                            AttackWithUnit(soldier, observation);
                        
                        }
                        else{
                            GroupUp(soldier, army, GROUP_DISTANCE, UNIT_TYPEID::ZERG_ZERGLING);
                        }
                    }
                    else if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISK) >= 8) {
                        if (CountNeibors(soldier, UNIT_TYPEID::ZERG_ZERGLING, GROUP_DISTANCE + 5.0f, observation) >= 8) {

                        }

                        else {
                            GroupUp(soldier, army, GROUP_DISTANCE, UNIT_TYPEID::ZERG_ZERGLING);
                        }
                    }
                    break;
                }

                case UNIT_TYPEID::ZERG_OVERLORD: {
                    const Unit* enemy = GetNearestEnemy(soldier->pos);
                    if (enemy == nullptr || Distance2D(soldier->pos,enemy->pos) < 20.0f) {
                        if (game_loop % 100 == 0) {
                            Point2D target = FindRandomLocation(observation->GetGameInfo());
                            Actions()->UnitCommand(soldier, ABILITY_ID::SMART, target);
                        }
                    }
                    else {
                        Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                        if (!bases.empty()) {
                            Point2D target = GetRandomEntry(bases)->pos;
                            Actions()->UnitCommand(soldier, ABILITY_ID::SMART, target);
                        }
                    }
                    break;
                 
                }
                case UNIT_TYPEID::ZERG_DRONE: {
                    if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) < 1) {
                        defend(soldier, observation);
                    }
                    break;
                }
                default:
                    break;
                }
            }

        return true;
    }

    bool GroupUp(const Unit* soldier, const Units army, float group_distance,UNIT_TYPEID unit_type_to_group) {
        Units farSoldier;
        for (const auto& other : army) {
            if (other->unit_type == unit_type_to_group && Distance2D(other->pos, soldier->pos) > group_distance) {
                farSoldier.push_back(other);
            }
        }
        const Unit* rally = GetNearestUnit(soldier->pos, farSoldier);
        if (rally != nullptr) {
            Actions()->UnitCommand(soldier, ABILITY_ID::ATTACK_ATTACK, rally->pos);
            return true;
        }
        return false;
    }

    bool defend(const Unit* unit, const ObservationInterface* observation) {
        Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            const Unit* nearest_enemy = GetNearestEnemy(base->pos);
            if (nearest_enemy != nullptr) {
                if (Distance2D(base->pos, nearest_enemy->pos) <= 20.0f) {
                    AttackWithUnit(unit, observation);
                }
            }
        }
        return false;
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

    bool AttackWithUnit(const Unit* unit, const ObservationInterface* observation, ABILITY_ID ability_id = ABILITY_ID::ATTACK_ATTACK) {
        Units enemy_unit = observation->GetUnits(Unit::Alliance::Enemy);
        if (enemy_unit.empty()) {
            GameInfo game_info = observation->GetGameInfo();
            Actions()->UnitCommand(unit, ability_id, game_info.enemy_start_locations.front());
        }
        else {
            const Unit* target = GetNearestEnemy(unit->pos);
            if (target->unit_type == UNIT_TYPEID::ZERG_CHANGELINGZERGLING || target->unit_type == UNIT_TYPEID::ZERG_CHANGELINGZERGLING || target->unit_type == UNIT_TYPEID::ZERG_CHANGELING) {
                Actions()->UnitCommand(unit, ABILITY_ID::SMART, target);
            }
            else {
                Actions()->UnitCommand(unit, ability_id, target->pos);
            }
        }
        return true;
    }

    bool TrainQueens(const ObservationInterface* observation) {
        //train queens if not enough are out.

        size_t queen_count = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY);
        Units queens = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_QUEEN));

        // find a hatchery that has no queens nearby
        if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1 && CountUnitType(UNIT_TYPEID::ZERG_QUEEN) < queen_count) {
            Units hatcheris = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_HATCHERY));
            // const Unit* choice_hatch = hatcheris.front();
            for (const auto& hatch : hatcheris) {
                bool has_queen = false;
                for (const auto& queen : queens) {
                    if (Distance2D(queen->pos, hatch->pos) < 10.0f) {
                        has_queen = true;
                        break;
                    }
                }
                if (!has_queen) {
                    for (const auto& order : hatch->orders) {
                        if (order.ability_id == ABILITY_ID::TRAIN_QUEEN) {
                            return false;
                        }
                    }
                    Actions()->UnitCommand(hatch, ABILITY_ID::TRAIN_QUEEN);
                }
                return true;

            }
        }
    }

    bool ManageQueens(const ObservationInterface* observation) {
        Units queens = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_QUEEN));
        for (const auto& queen : queens) {
            if (queen->energy > 25) {
                const Unit* hatch = GetNearestUnit(queen->pos, UNIT_TYPEID::ZERG_HATCHERY);
                if (GetRandomUnit(hatch, observation, UNIT_TYPEID::ZERG_HATCHERY)) {
                    Actions()->UnitCommand(queen, ABILITY_ID::EFFECT_INJECTLARVA, hatch);
                    return true;
                }
            }
        }

        return true;
    }

    bool ManageLarva(const ObservationInterface* observation) {
        size_t drone_count = 20;
        size_t overlord_count = 1;
        size_t ling_count = 0;
        size_t bang_count = 0;
        size_t roach_count = 0;
        size_t hydra_count = 0;

        size_t number_of_bases = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) + CountUnitType(UNIT_TYPEID::ZERG_LAIR);
        if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) >= 30) {
            drone_count = number_of_bases * 21;
        }
        drone_count = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) * 20;

        if (observation->GetFoodUsed() > observation->GetFoodCap() - 4) {
            overlord_count = (observation->GetFoodCap() / 6) + 1;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1) {
            ling_count = 20;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) > 10) {
            if (CountUnitType(UNIT_TYPEID::ZERG_BANELINGNEST) >= 1) {
                bang_count = 10;
            }
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_ROACHWARREN) >= 1) {
            roach_count = 20;
            ling_count = 30;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) >= 1) {
            ling_count = 20;
            hydra_count = 50;
        }
        
        TryTrainUnits(UNIT_TYPEID::ZERG_DRONE, ABILITY_ID::TRAIN_DRONE, drone_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_OVERLORD, ABILITY_ID::TRAIN_OVERLORD, overlord_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_HYDRALISK, ABILITY_ID::TRAIN_HYDRALISK, hydra_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_ZERGLING, ABILITY_ID::TRAIN_ZERGLING, ling_count, observation);
        TryTrainUnits(UNIT_TYPEID::ZERG_BANELING, ABILITY_ID::TRAIN_BANELING, bang_count, observation,UNIT_TYPEID::ZERG_BANELINGCOCOON,UNIT_TYPEID::ZERG_ZERGLING);

        return true;
    }

    bool TryTrainUnits(UNIT_TYPEID unit_type, ABILITY_ID ability_type, size_t number_of_units, const ObservationInterface* observation, UNIT_TYPEID morph_type = UNIT_TYPEID::ZERG_EGG, UNIT_TYPEID unit_to_morph = UNIT_TYPEID::ZERG_LARVA) {
        size_t number_existing_units = CountUnitType(unit_type);
        Units eggs = observation->GetUnits(Unit::Alliance::Self, IsUnit(morph_type));
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
            if (GetRandomUnit(larva, observation, unit_to_morph)) {
                Actions()->UnitCommand(larva, ability_type);
                return true;
            }
            return false;
        }
        return true;
    }

    bool ManageStructures(const ObservationInterface* observation) {
        if (CountUnitType(UNIT_TYPEID::ZERG_DRONE) >= 16) {
            if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
                TryBuildStructure(ABILITY_ID::BUILD_SPAWNINGPOOL);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_EXTRACTOR) < (CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) + CountUnitType(UNIT_TYPEID::ZERG_LAIR)) * 2) {
                Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                for (const auto& base : bases) {
                    TryBuildGas(base->pos);
                }
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_BANELINGNEST) < 1) {
                TryBuildStructure(ABILITY_ID::BUILD_BANELINGNEST);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_LAIR) < 1) {
                const Unit* hatch = nullptr;
                if (GetRandomUnit(hatch, observation, UNIT_TYPEID::ZERG_HATCHERY)) {
                    Actions()->UnitCommand(hatch, ABILITY_ID::MORPH_LAIR);
                }
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) < 1 && CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) > 20) {
                TryBuildStructure(ABILITY_ID::BUILD_HYDRALISKDEN);
            }
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_LARVA) <= 1 && observation->GetMinerals() >= 400 && CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) < 5) {
            TryBuildExpantion();
        }
        return true;
    }

    bool TryBuildGas(Point2D base_location) {
        const ObservationInterface* observation = Observation();
        Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser());

        float minimum_distance = 15.0f;
        Tag closestGeyser = 0;
        for (const auto& geyser : geysers) {
            float curr_distance = Distance2D(base_location, geyser->pos);
            if (curr_distance < minimum_distance) {
                if (Query()->Placement(ABILITY_ID::BUILD_EXTRACTOR, geyser->pos)) {
                    minimum_distance = curr_distance;
                    closestGeyser = geyser->tag;
                }
            }
        }

        if (closestGeyser == 0) {
            return false;
        }

        TryBuildStructure(ABILITY_ID::BUILD_EXTRACTOR, UNIT_TYPEID::ZERG_DRONE, closestGeyser);
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

    bool TryBuildStructure(ABILITY_ID ability_Type_for_structure, UNIT_TYPEID unit_type, Tag location_tag) {
        const ObservationInterface* observation = Observation();
        Units workers = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
        const Unit* target = observation->GetUnit(location_tag);

        if (workers.empty()) {
            return false;
        }

        for (const auto& worker : workers) {
            for (const auto& order : worker->orders) {
                if (order.ability_id == ability_Type_for_structure) {
                    return false;
                }
            }
        }

        const Unit* unit = GetRandomEntry(workers);

        if (Query()->Placement(ability_Type_for_structure, target->pos)) {
            Actions()->UnitCommand(unit, ability_Type_for_structure, target);
            return true;
        }
        return false;
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
        Units townHalls = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
        Units gases = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_EXTRACTOR));

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
                    const Unit* other_hive = GetRandomEntry(townHalls);
                    Actions()->UnitCommand(drone, ABILITY_ID::SMART, other_hive->pos);
                }
            }
        }

        for (const auto& gas : gases) {
            if (gas->ideal_harvesters == 0) {
                continue;
            }

            if (gas->assigned_harvesters <= gas->ideal_harvesters) {
                const Unit* drone = nullptr;
                if (NearestDrone(drone, gas, observation)) {
                    Actions()->UnitCommand(drone,ABILITY_ID::SMART,gas);
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
        return true;
    }

    Point2D NearestExpantion(Point2D start, const ObservationInterface* observation) {
        float min_distance = std::numeric_limits<float>::max();
        Point3D closest_expantion(0,0,0);
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

        if (closest_expantion == Point3D(0, 0, 0)) {
            const Unit* overlord = nullptr;
            if (GetRandomUnit(overlord, observation, UNIT_TYPEID::ZERG_OVERLORD)) {
                float rx = GetRandomScalar();
                float ry = GetRandomScalar();
                return Point2D(overlord->pos.x + rx * 15.0f, overlord->pos.y + ry * 15.0f);
            }
        }
        return Point2D(closest_expantion.x,closest_expantion.y);
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

    const Unit* GetNearestEnemy(const Point2D& start) {
        Units units = Observation()->GetUnits(Unit::Alliance::Enemy);
        return GetNearestUnit(start, units);
    }

    const Unit* GetNearestUnit(const Point2D& start, UNIT_TYPEID unit_type, Unit::Alliance unit_ally = Unit::Alliance::Self) {
        Units units = Observation()->GetUnits(unit_ally, IsUnit(unit_type));
        return GetNearestUnit(start, units);
    }

    const Unit* GetNearestUnit(const Point2D& start, Units unit_list) {
        float distance = std::numeric_limits<float>::max();
        const Unit* target = nullptr;
        for (const auto& unit : unit_list) {
            float unit_distance = DistanceSquared2D(unit->pos, start);
            if (unit_distance < distance) {
                distance = unit_distance;
                target = unit;
            }
        }
        return target;
    }
};


