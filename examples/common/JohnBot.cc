//{C++}
#include <sc2api/sc2_api.h>
#include <sc2lib/sc2_lib.h>

#include <vector>
#include <algorithm>
#include <limits>

#include <cstdlib>
#include <ctime>

#include <iostream>
using namespace sc2;

struct IsCloked {
    bool operator()(const Unit& unit) {
        switch (unit.cloak)
        {
        case Unit::CloakState::Cloaked: return true;
        case Unit::CloakState::CloakedDetected: return true;
        default: return false;
            break;
        }
    }
};



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

struct IsAttack{
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
        // all units that can attack
         // terran
   //     case UNIT_TYPEID::TERRAN_SCV: return true;
        case UNIT_TYPEID::TERRAN_AUTOTURRET: return true;
        case UNIT_TYPEID::TERRAN_BANSHEE: return true;
        case UNIT_TYPEID::TERRAN_BATTLECRUISER: return true;
        case UNIT_TYPEID::TERRAN_BUNKER: return true;
        case UNIT_TYPEID::TERRAN_CYCLONE: return true;
        case UNIT_TYPEID::TERRAN_GHOST: return true;
        case UNIT_TYPEID::TERRAN_HELLION: return true;
        case UNIT_TYPEID::TERRAN_HELLIONTANK: return true;
        case UNIT_TYPEID::TERRAN_LIBERATOR: return true;
        case UNIT_TYPEID::TERRAN_LIBERATORAG: return true;
        case UNIT_TYPEID::TERRAN_MARAUDER: return true;
        case UNIT_TYPEID::TERRAN_MARINE: return true;
        case UNIT_TYPEID::TERRAN_MISSILETURRET: return true;
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;
        case UNIT_TYPEID::TERRAN_REAPER: return true;
        case UNIT_TYPEID::TERRAN_SIEGETANK: return true;
        case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED: return true;
        case UNIT_TYPEID::TERRAN_THOR: return true;
        case UNIT_TYPEID::TERRAN_THORAP: return true;
        case UNIT_TYPEID::TERRAN_VIKINGASSAULT: return true;
        case UNIT_TYPEID::TERRAN_VIKINGFIGHTER: return true;
        case UNIT_TYPEID::TERRAN_WIDOWMINE: return true;
        case UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED: return true;

    //    case UNIT_TYPEID::ZERG_DRONE: return true;
        case UNIT_TYPEID::ZERG_BANELING: return true;
        case UNIT_TYPEID::ZERG_BANELINGBURROWED: return true;
        case UNIT_TYPEID::ZERG_BROODLING: return true;
        case UNIT_TYPEID::ZERG_BROODLORD: return true;
        case UNIT_TYPEID::ZERG_CORRUPTOR: return true;
        case UNIT_TYPEID::ZERG_HYDRALISK: return true;
        case UNIT_TYPEID::ZERG_INFESTORTERRAN: return true;
        case UNIT_TYPEID::ZERG_LOCUSTMP: return true;
        case UNIT_TYPEID::ZERG_LOCUSTMPFLYING: return true;
        case UNIT_TYPEID::ZERG_LURKERMP: return true;
        case UNIT_TYPEID::ZERG_LURKERMPBURROWED: return true;
        case UNIT_TYPEID::ZERG_MUTALISK: return true;
        case UNIT_TYPEID::ZERG_QUEEN: return true;
        case UNIT_TYPEID::ZERG_RAVAGER: return true;
        case UNIT_TYPEID::ZERG_ROACH: return true;
        case UNIT_TYPEID::ZERG_ROACHBURROWED: return true;
        case UNIT_TYPEID::ZERG_SPINECRAWLER: return true;
        case UNIT_TYPEID::ZERG_SPORECRAWLER: return true;
        case UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP: return true;
        case UNIT_TYPEID::ZERG_SWARMHOSTMP: return true;
        case UNIT_TYPEID::ZERG_ULTRALISK: return true;
        case UNIT_TYPEID::ZERG_ZERGLING: return true;

      //  case UNIT_TYPEID::PROTOSS_PROBE: return true;
        case UNIT_TYPEID::PROTOSS_ADEPT: return true;
        case UNIT_TYPEID::PROTOSS_ARCHON: return true;
        case UNIT_TYPEID::PROTOSS_CARRIER: return true;
        case UNIT_TYPEID::PROTOSS_COLOSSUS: return true;
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR: return true;
        case UNIT_TYPEID::PROTOSS_DISRUPTOR: return true;
        case UNIT_TYPEID::PROTOSS_DISRUPTORPHASED: return true;
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR: return true;
        case UNIT_TYPEID::PROTOSS_IMMORTAL: return true;
        case UNIT_TYPEID::PROTOSS_INTERCEPTOR: return true;
        case UNIT_TYPEID::PROTOSS_MOTHERSHIP: return true;
        case UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE: return true;
        case UNIT_TYPEID::PROTOSS_ORACLE: return true;
        case UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP: return true;
        case UNIT_TYPEID::PROTOSS_PHOTONCANNON: return true;
        case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED: return true;
        case UNIT_TYPEID::PROTOSS_SENTRY: return true;
        case UNIT_TYPEID::PROTOSS_STALKER: return true;
        case UNIT_TYPEID::PROTOSS_TEMPEST: return true;
        case UNIT_TYPEID::PROTOSS_VOIDRAY: return true;
        case UNIT_TYPEID::PROTOSS_ZEALOT: return true;

        default: return false;
        }
    }
};



struct IsArmy {
    bool operator()(const Unit& unit) {
        switch (unit.unit_type.ToType()) {
            // all units that can attack
            // terran

        case UNIT_TYPEID::ZERG_BANELING: return true;
        case UNIT_TYPEID::ZERG_BANELINGBURROWED: return true;
        case UNIT_TYPEID::ZERG_BROODLING: return true;
        case UNIT_TYPEID::ZERG_BROODLORD: return true;
        case UNIT_TYPEID::ZERG_CORRUPTOR: return true;
        case UNIT_TYPEID::ZERG_HYDRALISK: return true;
        case UNIT_TYPEID::ZERG_INFESTORTERRAN: return true;
        case UNIT_TYPEID::ZERG_LOCUSTMP: return true;
        case UNIT_TYPEID::ZERG_LOCUSTMPFLYING: return true;
        case UNIT_TYPEID::ZERG_LURKERMP: return true;
        case UNIT_TYPEID::ZERG_LURKERMPBURROWED: return true;
        case UNIT_TYPEID::ZERG_MUTALISK: return true;
        case UNIT_TYPEID::ZERG_RAVAGER: return true;
        case UNIT_TYPEID::ZERG_ROACH: return true;
        case UNIT_TYPEID::ZERG_ROACHBURROWED: return true;
        case UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP: return true;
        case UNIT_TYPEID::ZERG_SWARMHOSTMP: return true;
        case UNIT_TYPEID::ZERG_ULTRALISK: return true;
        case UNIT_TYPEID::ZERG_ZERGLING: return true;

        default: return false;
        }
    }
};

struct BaseBuildings {
    bool spawning_pool;
    bool roach_warren;
    bool lair;
    bool hydralisk_den;
};

// use this in a priority queue to organize commands to use resources
class BuildCommand {
private:
    UNIT_TYPEID m_worker;
    ABILITY_ID m_command;

    size_t m_mineral_cost;
    size_t m_gas_cost;

    size_t m_priority;

public:
    BuildCommand(ABILITY_ID command, UNIT_TYPEID worker_type, size_t mineral_cost, size_t gas_cost, size_t priority) {
        m_command = command;
        m_worker = worker_type;
        m_priority = priority;
        m_mineral_cost = mineral_cost;
        m_gas_cost = gas_cost;
    }

    bool operator<(const BuildCommand& b) const {
        return m_priority < b.m_priority;
    }
    bool operator>(const BuildCommand& b) const {
        return m_priority > b.m_priority;
    }
    bool operator<=(const BuildCommand& b) const {
        return m_priority <= b.m_priority;
    }
    bool operator>=(const BuildCommand& b) const {
        return m_priority >= b.m_priority;
    }

    void SetBuilder(UNIT_TYPEID builder) {
        m_worker = builder;
    }

    void IncreasePriority(size_t value) {
        m_priority += value;
    }

    void DecreasePriority(size_t value) {
        m_priority -= value;
    }

    size_t GetMineralCost() {
        return m_mineral_cost;
    }

    size_t GetGasCost() {
        return m_gas_cost;
    }

    ABILITY_ID GetCommand() {
        return m_command;
    }

     UNIT_TYPEID GetWorker() {
        return m_worker;
    }

};

class JohnBot : public Agent {
public:
    virtual void OnGameStart() final {
        stage = 0;
        group_size = 6;
        attack_target = Observation()->GetGameInfo().enemy_start_locations.front();
        build_queue.clear();

    }

    virtual void OnStep() {
        const ObservationInterface* observation = Observation();
        uint32_t game_loop = Observation()->GetGameLoop();

       // std::cout << "Larva" << std::endl;
       // ManageLarva(observation);

        //std::cout << "structures" << std::endl;
        //ManageStructures(observation);

        ManageProduction(observation);
        ManageQueue(observation);
       
        ManageMining(observation);

//        TrainQueens(observation);
        ManageQueens(observation);

        ManageArmy(observation);
        /*
        switch (game_loop % 10) {
        case 0: {
            ManageLarva(observation);
            break;
        }
        case 1: {
            ManageStructures(observation);
            break;
        }
        case 3: {
            ManageMining(observation);
            break;
        }
        case 4: {
            TrainQueens(observation);
            ManageQueens(observation);
            break;
        }
        case 5: {
//            ManageArmy(observation);
            break;
        }
        default:
            break;
        }
    */

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
           
              //  Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST);
           
            break;
        }
        case UNIT_TYPEID::ZERG_BANELINGNEST: {
            if (CountUnitType(UNIT_TYPEID::ZERG_LAIR)) {
               // Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS);
            }
            break;
        }
        case UNIT_TYPEID::ZERG_HYDRALISKDEN: {
           // Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_MUSCULARAUGMENTS);
            break;
        }
        default: {
            break;
        }
        }
    }

    virtual void OnUnitEnterVision(const Unit* enemy) {
        bool attack = false;
        Units bases = Observation()->GetUnits(Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            if (Distance2D(enemy->pos, base->pos) <= 30) {
                attack = true;
            }
        }
        if (attack) {
            Units army = Observation()->GetUnits(Unit::Alliance::Self, IsArmy());
            for (const auto& soldier : army) {
                Actions()->UnitCommand(soldier, ABILITY_ID::ATTACK_ATTACK, enemy->pos);
            }

        }
    }

    virtual void OnUnitDestroyed(const Unit*  unit) {
        switch (unit->unit_type.ToType()) {
        case UNIT_TYPEID::ZERG_HATCHERY:
        case UNIT_TYPEID::ZERG_LAIR:
            // command all drones to stop
            Units drones = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_DRONE));
            Units townHalls = Observation()->GetUnits(Unit::Alliance::Self, IsTownHall());

            std::cout << "a base was destroyed" << std::endl;

            for (const auto& drone : drones) {
                const Unit* base = GetNearestUnit(drone->pos, townHalls);
                if (base != nullptr) {
                    Actions()->UnitCommand(drone, ABILITY_ID::SMART, base->pos);
                }
            }
            break;

        }
    }

private:
    int stage;
    size_t group_size;
    Point2D attack_target;// = game_info.enemy_start_locations.front();

    // queue
    std::vector<BuildCommand> build_queue;

    size_t CountUnitType(UNIT_TYPEID unit_type) {
        return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
    }

    size_t CountUnitTypeTrain(ABILITY_ID ability_type, const ObservationInterface* observation, UNIT_TYPEID morph_type = UNIT_TYPEID::ZERG_EGG, UNIT_TYPEID unit_to_morph = UNIT_TYPEID::ZERG_LARVA) {
        size_t number_existing_units = 0;
        Units eggs = observation->GetUnits(Unit::Alliance::Self, IsUnit(morph_type));
        for (const auto& egg : eggs) {
            if (!egg->orders.empty()) {
                if (egg->orders.front().ability_id == ability_type) {// || morph_type == UNIT_TYPEID::ZERG_OVERLORDCOCOON || morph_type == UNIT_TYPEID::ZERG_BANELINGCOCOON) {
                    number_existing_units++;
                }
            }
        }
        return number_existing_units;
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
            Units army = observation->GetUnits(Unit::Alliance::Self, IsArmy());
            Units badies = observation->GetUnits(Unit::Alliance::Enemy, IsAttack());
            Point3D rally = AverageLocation(army);

            for (const auto& soldier : army) {
                const float GROUP_DISTANCE = 10.0f;
                size_t neibors = CountNeibors(soldier, army, GROUP_DISTANCE, observation);
                size_t enemies = badies.size();


                switch (soldier->unit_type.ToType())
                {
                case UNIT_TYPEID::ZERG_OVERSEER: {
                    //Units cloked_units = observation->GetUnits(Unit::Alliance::Enemy);
                    //GroupUp(soldier, army, GROUP_DISTANCE / 8, ABILITY_ID::SMART);
                    const Unit* near = GetNearestUnit(soldier->pos, army);
                    GroupUp(soldier, GROUP_DISTANCE, rally);
                    break;
                }
                case UNIT_TYPEID::ZERG_ZERGLING: {
                    const Unit* near_enemy = GetNearestUnit(soldier->pos, badies);
                    if (near_enemy != nullptr && Distance2D(near_enemy->pos, soldier->pos) <= GROUP_DISTANCE) {
                        if (near_enemy->unit_type == UNIT_TYPEID::ZERG_BANELING || neibors < enemies) {
                            Units townHalls = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                            const Unit* base = GetRandomEntry(townHalls);
                            Actions()->UnitCommand(soldier, ABILITY_ID::MOVE, base->pos);
                        }
                        else {
                            AttackWithUnit(soldier, observation);
                        }
                    }
                    else if (neibors >= group_size) {
                        AttackWithUnit(soldier, observation);
                    }
                    else {
                       // GroupUp(soldier, army, GROUP_DISTANCE / 4);
                        GroupUp(soldier, GROUP_DISTANCE, rally);
                    }
                    break;
                }

                default: {

                    const Unit* near_enemy = GetNearestUnit(soldier->pos,badies);
                    if (near_enemy != nullptr && Distance2D(near_enemy->pos, soldier->pos) <= GROUP_DISTANCE * 8 && neibors > enemies) {
                        if ( neibors < enemies) {
                            Units townHalls = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                            const Unit* base = GetRandomEntry(townHalls);
                            Actions()->UnitCommand(soldier, ABILITY_ID::MOVE, base->pos);
                        }
                        else {
                            AttackWithUnit(soldier, observation);
                        }
                    }
                    else if (neibors >= group_size) {
                        AttackWithUnit(soldier, observation);
                    }
                    else {
                        GroupUp(soldier, GROUP_DISTANCE, rally);
                    }
                    break;
                }
                }
                
            }

        return true;
    }

    Point3D AverageLocation(Units group) {
        Point3D average;
        average.x = 0;
        average.y = 0;
        average.z = 0;
        for (const auto& unit : group) {
            average += unit->pos;
        }
        average.x /= group.size();
        average.y /= group.size();
        average.z /= group.size();
        return average;
    }
    bool GroupUp(const Unit* soldier, float distance, Point3D rally) {
        if (Distance2D(soldier->pos, rally) > distance) {
            Actions()->UnitCommand(soldier, ABILITY_ID::ATTACK_ATTACK, rally);
            return true;
        }
        return false;
    }

    // Groups up to spesific unit type in the units group
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

    // Groups up to any other unit in the set of units
    bool GroupUp(const Unit* soldier, const Units army, float group_distance,ABILITY_ID ability = ABILITY_ID::ATTACK_ATTACK) {
        //if (Observation()->GetGameLoop() % 20 != 0) return true;
        const Unit* rally = nullptr;

        float distance = std::numeric_limits<float>::max();
        for (const auto& other : army) {
            float next = Distance2D(other->pos, soldier->pos);
            if (next > group_distance && next < distance) {
                rally = other;
                Actions()->UnitCommand(soldier, ability, rally->pos);
                return true;
            }
        }
        return false;
        /*
        if (rally != nullptr) {
            
        }
        */
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

    bool defend(const Unit* unit, const Unit*& enemy_unit, const ObservationInterface* observation, size_t distance = 30) {
        enemy_unit = GetNearestEnemy(unit->pos);

        if (enemy_unit == nullptr) {
            return false;
        }

        if (Distance2D(unit->pos, enemy_unit->pos)) {

        }

        return false;
    }

    // Counts the units of a specific type nearby
    size_t CountNeibors(const Unit* unit, UNIT_TYPEID unit_type, float distance, const ObservationInterface* observation) {
        Units freinds = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));

        return CountNeibors(unit,freinds,distance,observation);
    }

    // Count the units within the units group
    size_t CountNeibors(const Unit* unit, Units army, float distance, const ObservationInterface* observation) {
        size_t neibors = 0;

        for (const auto& freind : army) {
            if (Distance2D(freind->pos, unit->pos) <= distance) {
                neibors++;
            }
        }
        return neibors;
    }

    // Counts the number of enemy units nearby
    size_t CountEnemies(const Unit* unit, UNIT_TYPEID unit_type, float distance, const ObservationInterface* observation) {
        Units enimies = observation->GetUnits(Unit::Alliance::Enemy, IsUnit(unit_type));
        return CountNeibors(unit, enimies, distance, observation);
    }

    bool AttackWithUnit(const Unit* unit, const ObservationInterface* observation, ABILITY_ID ability_id = ABILITY_ID::ATTACK_ATTACK) {
        Units enemy_unit = observation->GetUnits(Unit::Alliance::Enemy);
        if (enemy_unit.empty()) {
            GameInfo game_info = observation->GetGameInfo();
            Units minerals = observation->GetUnits(Unit::Alliance::Neutral);

            Units units = observation->GetUnits(Unit::Alliance::Self);
            const Unit* nearest_scout = GetNearestUnit(attack_target, units);
            if (Distance2D(nearest_scout->pos, attack_target) <= 5.0f) {
                attack_target = GetRandomEntry(minerals)->pos;
            }
            Actions()->UnitCommand(unit, ability_id, attack_target);
        }
        else {
            const Unit* target = GetNearestEnemy(unit->pos);
            if (target->unit_type == UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS || target->unit_type == UNIT_TYPEID::ZERG_CHANGELINGZERGLING || target->unit_type == UNIT_TYPEID::ZERG_CHANGELING) {
                Actions()->UnitCommand(unit, ABILITY_ID::SMART, target);
            }
            else {
                Actions()->UnitCommand(unit, ability_id, target->pos);
            }
        }
        return true;
    }

    /*
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
    */

    bool ManageQueens(const ObservationInterface* observation) {
        Units queens = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_QUEEN));
        for (const auto& queen : queens) {
            if (queen->energy > 25) {
                Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                for(const auto& base : bases) {
                    if (base->build_progress != 1) {
                        continue;
                    }
                    if (queen->energy >= 25 && queen->orders.empty()) {
                        Actions()->UnitCommand(queen, ABILITY_ID::EFFECT_INJECTLARVA, base);
                    }
                }
            }
        }
        return true;
    }

    bool ContainBuff(std::vector<BuffID> buffs, BuffID buff_id) {
        for (const auto& buff : buffs) {
            if (buff == buff_id) {
                return true;
            }
        }
        return false;
    }

    size_t CountBuilding(UNIT_TYPEID type) {
        const ObservationInterface* observation = Observation();

        size_t number_existing_units = 0;
        Units buildings = observation->GetUnits(Unit::Alliance::Self, IsUnit(type));
        for (const auto& building : buildings) {
            if (building->build_progress < 1) {
                number_existing_units++;
            }
        }
        return number_existing_units;
    }

    bool ManageProduction(const ObservationInterface* observation) {

        size_t optimal_evolution = 1;
        size_t number_evolution = CountUnitType(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) + CountUnitTypeTrain(ABILITY_ID::BUILD_EVOLUTIONCHAMBER, observation, UNIT_TYPEID::ZERG_DRONE) + UnitsInQueue(ABILITY_ID::BUILD_EVOLUTIONCHAMBER);

        size_t optimal_zerglings = 32;
        size_t number_zerglings = CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) + CountUnitTypeTrain(ABILITY_ID::TRAIN_ZERGLING, observation) * 2;


        size_t optimal_overlords = (observation->GetFoodUsed() / 8) + 1;
        size_t number_overlords = CountUnitType(UNIT_TYPEID::ZERG_OVERLORD) + CountUnitTypeTrain(ABILITY_ID::TRAIN_OVERLORD, observation);

        size_t optimal_hatcheries = (CountUnitType(UNIT_TYPEID::ZERG_DRONE) / 19) + 1;
        size_t number_hatcheries = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) + CountUnitType(UNIT_TYPEID::ZERG_LAIR) + CountUnitTypeTrain(ABILITY_ID::BUILD_HATCHERY, observation, UNIT_TYPEID::ZERG_DRONE);

        uint32_t game_loop = Observation()->GetGameLoop();
        group_size = (game_loop / 742);
        if (group_size > 60) {
            group_size = 60;
        }

        size_t optimal_queens = optimal_hatcheries;
        size_t number_queens = CountUnitType(UNIT_TYPEID::ZERG_QUEEN) + CountUnitTypeTrain(ABILITY_ID::TRAIN_QUEEN, observation, UNIT_TYPEID::ZERG_HATCHERY);

        bool has_spawning_pool = CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) - CountBuilding(UNIT_TYPEID::ZERG_SPAWNINGPOOL);

        bool has_roach_warren = CountUnitType(UNIT_TYPEID::ZERG_ROACHWARREN) - CountBuilding(UNIT_TYPEID::ZERG_ROACHWARREN);

        size_t optimal_roaches = 60;
        size_t number_roaches = CountUnitType(UNIT_TYPEID::ZERG_ROACH) + CountUnitTypeTrain(ABILITY_ID::TRAIN_ROACH, observation);

        bool has_lair = CountUnitType(UNIT_TYPEID::ZERG_LAIR) - CountBuilding(UNIT_TYPEID::ZERG_LAIR);

        bool has_hydralisk_den = CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) - CountBuilding(UNIT_TYPEID::ZERG_HYDRALISKDEN);

        size_t optimal_hydralisk = 60;
        size_t number_hydralisk = CountUnitType(UNIT_TYPEID::ZERG_HYDRALISK) + CountUnitTypeTrain(ABILITY_ID::TRAIN_HYDRALISK, observation);

        size_t optimal_overseer = 2;
        size_t number_overseer = CountUnitType(UNIT_TYPEID::ZERG_OVERSEER) + CountUnitTypeTrain(ABILITY_ID::MORPH_OVERSEER, observation) + CountUnitTypeTrain(ABILITY_ID::MORPH_OVERSEER,observation,UNIT_TYPEID::ZERG_OVERLORDCOCOON);

        bool has_spire = CountUnitType(UNIT_TYPEID::ZERG_SPIRE) - CountBuilding(UNIT_TYPEID::ZERG_SPIRE);

        size_t optimal_mutalisk = 40;
        size_t number_mutalisk = CountUnitType(UNIT_TYPEID::ZERG_MUTALISK) + CountUnitTypeTrain(ABILITY_ID::TRAIN_MUTALISK, observation) + CountUnitTypeTrain(ABILITY_ID::TRAIN_MUTALISK, observation);

        size_t optimal_corruptor = 20;
        size_t number_corruptor = CountUnitType(UNIT_TYPEID::ZERG_MUTALISK) + CountUnitTypeTrain(ABILITY_ID::TRAIN_CORRUPTOR, observation) + CountUnitTypeTrain(ABILITY_ID::TRAIN_CORRUPTOR, observation);

        size_t optimal_drones = 16 + observation->GetUnits(Unit::Alliance::Self, IsArmy()).size(); // number of drones needed;
        size_t number_drones = CountUnitType(UNIT_TYPEID::ZERG_DRONE) + CountUnitTypeTrain(ABILITY_ID::TRAIN_DRONE, observation);

        size_t optimal_gas = ((number_drones)/16) + 1;
        size_t number_gas = CountUnitType(UNIT_TYPEID::ZERG_EXTRACTOR) + CountUnitTypeTrain(ABILITY_ID::BUILD_EXTRACTOR, observation, UNIT_TYPEID::ZERG_DRONE) + UnitsInQueue(ABILITY_ID::BUILD_EXTRACTOR);
        // use the queue to spend money

        // add Drones if not enough
        // grow the queue
        // overlords
        if (number_overlords + UnitsInQueue(ABILITY_ID::TRAIN_OVERLORD) < optimal_overlords) {
            AddBuildCommand(ABILITY_ID::TRAIN_OVERLORD, UNIT_TYPEID::ZERG_LARVA, 100, 0, 1000);

        }
        // dones
        if (number_drones + UnitsInQueue(ABILITY_ID::TRAIN_DRONE) < optimal_drones) {
            AddBuildCommand(ABILITY_ID::TRAIN_DRONE, UNIT_TYPEID::ZERG_LARVA, 50, 0, 112);
            
        }
        // hatcheries
        if (number_hatcheries + UnitsInQueue(ABILITY_ID::BUILD_HATCHERY) < optimal_hatcheries) {
            AddBuildCommand(ABILITY_ID::BUILD_HATCHERY, UNIT_TYPEID::ZERG_DRONE, 300, 0, 120);

        }
        // queens
        if (number_queens + UnitsInQueue(ABILITY_ID::TRAIN_QUEEN) < optimal_queens && has_spawning_pool) {
            AddBuildCommand(ABILITY_ID::TRAIN_QUEEN, UNIT_TYPEID::ZERG_HATCHERY, 100, 0, 200);
        }
        // gas
        if (number_gas < optimal_gas && number_drones > 16) {
            TryBuildGas();

            // BuildCommand n_command(ABILITY_ID::BUILD_EXTRACTOR, UNIT_TYPEID::ZERG_DRONE, 75, 0, 100);
           // build_queue.push_back(n_command);
        }
        // evolution chamber
        if (number_evolution < optimal_evolution) {
            AddBuildCommand(ABILITY_ID::BUILD_EVOLUTIONCHAMBER, UNIT_TYPEID::ZERG_DRONE, 75, 0, 108);
        }
        // spawning pool
        if (!(CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) + UnitsInQueue(ABILITY_ID::BUILD_SPAWNINGPOOL) + CountUnitTypeTrain(ABILITY_ID::BUILD_SPAWNINGPOOL,observation,UNIT_TYPEID::ZERG_DRONE))) {
            AddBuildCommand(ABILITY_ID::BUILD_SPAWNINGPOOL, UNIT_TYPEID::ZERG_DRONE, 200, 0, 1010);
            
        }
        // roach warren
        if (has_spawning_pool && !(CountUnitType(UNIT_TYPEID::ZERG_ROACHWARREN) + UnitsInQueue(ABILITY_ID::BUILD_ROACHWARREN)+ CountUnitTypeTrain(ABILITY_ID::BUILD_ROACHWARREN,observation,UNIT_TYPEID::ZERG_DRONE))) {
            AddBuildCommand(ABILITY_ID::BUILD_ROACHWARREN, UNIT_TYPEID::ZERG_DRONE, 150, 0, 112);
            
        }
        // liar
        if (has_spawning_pool && !(CountUnitType(UNIT_TYPEID::ZERG_LAIR) + UnitsInQueue(ABILITY_ID::MORPH_LAIR) + CountUnitTypeTrain(ABILITY_ID::MORPH_LAIR, observation,UNIT_TYPEID::ZERG_HATCHERY))) {
            AddBuildCommand(ABILITY_ID::MORPH_LAIR, UNIT_TYPEID::ZERG_HATCHERY, 150, 100, 102);
        }
        // hydralisk den
        if (has_lair && !(CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) + UnitsInQueue(ABILITY_ID::BUILD_HYDRALISKDEN) + CountUnitTypeTrain(ABILITY_ID::BUILD_HYDRALISKDEN,observation,UNIT_TYPEID::ZERG_DRONE))) {
            AddBuildCommand(ABILITY_ID::BUILD_HYDRALISKDEN, UNIT_TYPEID::ZERG_DRONE, 150, 100, 128);
        }
        // spire
        if (has_lair && !(CountUnitType(UNIT_TYPEID::ZERG_SPIRE) + UnitsInQueue(ABILITY_ID::BUILD_SPIRE) + CountUnitTypeTrain(ABILITY_ID::BUILD_SPIRE, observation, UNIT_TYPEID::ZERG_DRONE))) {
            AddBuildCommand(ABILITY_ID::BUILD_SPIRE, UNIT_TYPEID::ZERG_DRONE, 200, 200, 138);
        }
        // zergling
        if (has_spawning_pool && (number_zerglings + UnitsInQueue(ABILITY_ID::TRAIN_ZERGLING) * 2) < optimal_zerglings) {
            AddBuildCommand(ABILITY_ID::TRAIN_ZERGLING, UNIT_TYPEID::ZERG_LARVA, 50, 0, 112);
        }
        // roach
        if (has_roach_warren  && (number_roaches + UnitsInQueue(ABILITY_ID::TRAIN_ROACH)) < optimal_roaches) {
            AddBuildCommand(ABILITY_ID::TRAIN_ROACH, UNIT_TYPEID::ZERG_LARVA, 75, 25, 112);
        }
        // hydralisk
        if (has_hydralisk_den && (number_hydralisk + UnitsInQueue(ABILITY_ID::TRAIN_HYDRALISK)) < optimal_hydralisk) {
            AddBuildCommand(ABILITY_ID::TRAIN_HYDRALISK, UNIT_TYPEID::ZERG_LARVA, 100, 100, 112);
        }
        // overseer
        if (has_lair && (number_overseer + UnitsInQueue(ABILITY_ID::MORPH_OVERSEER)) < optimal_overseer) {
          //  AddBuildCommand(ABILITY_ID::MORPH_OVERSEER, UNIT_TYPEID::ZERG_OVERLORD, 50, 50, 120);
        }
        // mutalisk
        if (has_spire && (number_mutalisk + UnitsInQueue(ABILITY_ID::TRAIN_MUTALISK)) < optimal_mutalisk) {
            AddBuildCommand(ABILITY_ID::TRAIN_MUTALISK, UNIT_TYPEID::ZERG_LARVA, 100, 100, 100);
        }
        // corruptors
        if (has_spire && (number_corruptor + UnitsInQueue(ABILITY_ID::TRAIN_CORRUPTOR)) < optimal_corruptor) {
            AddBuildCommand(ABILITY_ID::TRAIN_CORRUPTOR, UNIT_TYPEID::ZERG_LARVA, 100, 100, 100);
        }
        // upgrades
        auto upgrades = observation->GetUpgrades();
        // wing-ling
        if (has_spawning_pool && !HaveUpgrade(UPGRADE_ID::ZERGLINGMOVEMENTSPEED) && !UnitsInQueue(ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST) && !CountUnitTypeTrain(ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST, observation, UNIT_TYPEID::ZERG_SPAWNINGPOOL)) {
            AddBuildCommand(ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST, UNIT_TYPEID::ZERG_SPAWNINGPOOL, 100, 100, 200);
        }
        // fast roaches
        if (has_roach_warren && has_lair && !HaveUpgrade(UPGRADE_ID::GLIALRECONSTITUTION) && !UnitsInQueue(ABILITY_ID::RESEARCH_GLIALREGENERATION) && !CountUnitTypeTrain(ABILITY_ID::RESEARCH_GLIALREGENERATION, observation, UNIT_TYPEID::ZERG_ROACHWARREN)) {
            AddBuildCommand(ABILITY_ID::RESEARCH_GLIALREGENERATION, UNIT_TYPEID::ZERG_ROACHWARREN, 150, 150, 200);
        }
        // armor
        if (HasIdle(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && CountUnitType(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) - CountBuilding(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && !CountUnitTypeTrain(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, observation, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && !HaveUpgrade(UPGRADE_ID::ZERGGROUNDARMORSLEVEL1) && !UnitsInQueue(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR) && observation->GetVespene() >= 150) {
            AddBuildCommand(ABILITY_ID::RESEARCH_ZERGGROUNDARMOR, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, 150, 150, 200);
        }
        /*
        if (HasIdle(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && CountUnitType(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) - CountBuilding(UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && !CountUnitTypeTrain(ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS, observation, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER) && !HaveUpgrade(UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1) && !UnitsInQueue(ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS) && observation->GetVespene() >= 100) {
            AddBuildCommand(ABILITY_ID::RESEARCH_ZERGMELEEWEAPONS, UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, 100, 100, 150);
        }
        */
        // fast hydralisk
        if (has_hydralisk_den && !HaveUpgrade(UPGRADE_ID::EVOLVEMUSCULARAUGMENTS) && !UnitsInQueue(ABILITY_ID::RESEARCH_MUSCULARAUGMENTS) && !CountUnitTypeTrain(ABILITY_ID::RESEARCH_MUSCULARAUGMENTS,observation,UNIT_TYPEID::ZERG_HYDRALISKDEN)) {
            AddBuildCommand(ABILITY_ID::RESEARCH_MUSCULARAUGMENTS, UNIT_TYPEID::ZERG_HYDRALISKDEN, 100, 100, 200);
        }

        std::sort(build_queue.begin(), build_queue.end());

        BaseBuildings buildings;
        buildings.spawning_pool = has_spawning_pool;
        buildings.roach_warren = has_roach_warren;
        buildings.lair = has_lair;
  //      SanitizeQueue(buildings);

        return true;
    }

    bool HasIdle(UNIT_TYPEID unit_type) {
        Units units = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
        for (const auto& unit : units) {
            if (unit->orders.size() <= 0) {
                return true;
            }
        }
        return false;
    }

    bool HaveUpgrade(UPGRADE_ID upgrade) {
        auto upgs = Observation()->GetUpgrades();
        for (const auto& up : upgs) {
            if (up == upgrade) {
                return true;
            }
        }
        return false;
    }

    void AddBuildCommand(ABILITY_ID ability, UNIT_TYPEID worker, size_t mineral, size_t gas, size_t priority) {
        BuildCommand n_command(ability, worker, mineral, gas, priority);
        build_queue.push_back(n_command);
        std::random_shuffle(build_queue.begin(), build_queue.end());
    }

    /*
    bool ManageLarva(const ObservationInterface* observation) {
        

        size_t number_of_bases = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) + CountUnitType(UNIT_TYPEID::ZERG_LAIR);
        if (observation->GetUnits(Unit::Alliance::Self,IsArmy()).size() >= 20) {
            drone_count = 48;
        }
       // drone_count = CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) * 20;

        if (observation->GetFoodUsed() > observation->GetFoodCap() - 4) {
            overlord_count = (observation->GetFoodCap() / 6) + 1;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) >= 1) {
            ling_count = 20;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_ZERGLING) > 15 && stage > 0) {
            if (CountUnitType(UNIT_TYPEID::ZERG_BANELINGNEST) >= 1) {
                bang_count = 10;
            }
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_ROACHWARREN) >= 1 && stage > 0) {
            roach_count = 15;
            group_size = 40;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) >= 1 && stage > 1) {
            overseer_count = 2;
            ling_count = 10;
            roach_count = 20;
            hydra_count = 50;
            group_size = 50;
        }
        if (observation->GetMinerals() >= 100)
            TryTrainUnits(UNIT_TYPEID::ZERG_OVERLORD, ABILITY_ID::TRAIN_OVERLORD, overlord_count, observation);

        if (observation->GetMinerals() >= 50) {
            TryTrainUnits(UNIT_TYPEID::ZERG_DRONE, ABILITY_ID::TRAIN_DRONE, drone_count, observation);
            TryTrainUnits(UNIT_TYPEID::ZERG_ZERGLING, ABILITY_ID::TRAIN_ZERGLING, ling_count, observation);
        }
        if (observation->GetMinerals() >= 100 && observation->GetVespene() >= 50)
            TryTrainUnits(UNIT_TYPEID::ZERG_HYDRALISK, ABILITY_ID::TRAIN_HYDRALISK, hydra_count, observation);
        if (observation->GetMinerals() >= 75 && observation->GetVespene() >= 25)
            TryTrainUnits(UNIT_TYPEID::ZERG_ROACH, ABILITY_ID::TRAIN_ROACH, roach_count, observation);
        if (observation->GetMinerals() >= 25 && observation->GetVespene() >= 25)
            TryTrainUnits(UNIT_TYPEID::ZERG_BANELING, ABILITY_ID::TRAIN_BANELING, bang_count, observation,UNIT_TYPEID::ZERG_BANELINGCOCOON,UNIT_TYPEID::ZERG_ZERGLING);
        if (observation->GetMinerals() >= 50 && observation->GetVespene() >= 50)
            TryTrainUnits(UNIT_TYPEID::ZERG_OVERSEER, ABILITY_ID::MORPH_OVERSEER, overseer_count, observation, UNIT_TYPEID::ZERG_OVERLORDCOCOON, UNIT_TYPEID::ZERG_OVERLORD);

        return true;
    }
    */

    bool TryTrainUnits(UNIT_TYPEID unit_type, ABILITY_ID ability_type, size_t number_of_units, const ObservationInterface* observation, UNIT_TYPEID morph_type = UNIT_TYPEID::ZERG_EGG, UNIT_TYPEID unit_to_morph = UNIT_TYPEID::ZERG_LARVA) {
        size_t number_existing_units = CountUnitType(unit_type);
        Units eggs = observation->GetUnits(Unit::Alliance::Self, IsUnit(morph_type));
        for (const auto& egg : eggs) {
            if (!egg->orders.empty()) {
                if (egg->orders.front().ability_id == ability_type) {// || morph_type == UNIT_TYPEID::ZERG_OVERLORDCOCOON || morph_type == UNIT_TYPEID::ZERG_BANELINGCOCOON) {
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

    bool ManageQueue(const ObservationInterface* observation) {

        // pop from the queue to build

        unsigned index = build_queue.size() - 1;
        size_t gas = observation->GetVespene();
        size_t minerals = observation->GetMinerals();
       // std::cout << "new step" << std::endl;

        // move through the queue if you can build the first, build it, if not see if building the next is possible
        while (index < build_queue.size()) { // fix this
           // std::cout << index << std::endl;
            BuildCommand top = build_queue[index];
            UNIT_TYPEID builder = top.GetWorker();
            ABILITY_ID to_build = top.GetCommand();

            // treat lair as hatchery if no hatchery exists
            if (builder == UNIT_TYPEID::ZERG_HATCHERY) {
                if (!CountUnitType(UNIT_TYPEID::ZERG_HATCHERY)) {
                    top.SetBuilder(UNIT_TYPEID::ZERG_LAIR);
                }
            }

            // check cost
            if (minerals >= top.GetMineralCost() && observation->GetVespene() >= top.GetGasCost() && CountUnitType(builder) > 0) {
                // build the thing
                if (builder == UNIT_TYPEID::ZERG_DRONE) {
                    if (to_build == ABILITY_ID::BUILD_HATCHERY) {
                        TryBuildExpantion();
                        build_queue.erase(build_queue.begin() + index);
                        index--;
                        break;
                    
                    }
                    if (to_build == ABILITY_ID::BUILD_EXTRACTOR) {
                        if (TryBuildGas()) {
                            build_queue.erase(build_queue.begin() + index);
                            index--;
                            break;
                        }
                    }
                    else {
                        if (TryBuildStructure(to_build)) {
                            build_queue.erase(build_queue.begin() + index);
                            index--;
                            break;
                        }
                    }
                }

                else {
                    if (SpawnUnit(top, observation)) {
                        // pop top and clear memory
                        build_queue.erase(build_queue.begin() + index);
                        index--;
                        break;
                    }
                }
            }
            else if (CountUnitType(builder) <= 0) {
                index--;
                continue;
            }
            else {
                // check if you have excess minerals and waiting for gas
                if (minerals >= top.GetMineralCost()) {
                    index--;
                    minerals -= top.GetMineralCost();
                    continue;
                }
            }
            break;
        }

        return true;
    }

    bool SanitizeQueue(BaseBuildings buildings) {

        bool runnable = true;
        while (runnable) {
            ABILITY_ID command = build_queue.back().GetCommand();

            // don't build things that need a spawning pool if you don't have one
            if (!buildings.spawning_pool) {
                if (command == ABILITY_ID::TRAIN_ZERGLING || command == ABILITY_ID::TRAIN_QUEEN || command == ABILITY_ID::MORPH_LAIR || command == ABILITY_ID::BUILD_ROACHWARREN) {
                    build_queue.pop_back();
                    continue;
                }
            }
            // don't build things if they need a roach warren
            if (!buildings.roach_warren) {
                if (command == ABILITY_ID::TRAIN_ROACH) {
                    build_queue.pop_back();
                    continue;
                }
            }

            // if none else then move on
            runnable = false;
        }
        return true;
    }

    bool SpawnUnit(BuildCommand command, const ObservationInterface* observation) {
        // return false if not enough supply
        if (observation->GetFoodUsed() >= observation->GetFoodCap() && command.GetCommand() != ABILITY_ID::TRAIN_OVERLORD) {
            return false;
        }

        if (observation->GetMinerals() >= command.GetMineralCost() && observation->GetVespene() >= command.GetGasCost()) {
            const Unit* larva;
            if (GetRandomUnit(larva, observation, command.GetWorker())) {
                Actions()->UnitCommand(larva, command.GetCommand());
                    return true;
            }
        }
    
        return false;
    }

    // Disable for now
    /*
    bool ManageStructures(const ObservationInterface* observation) {
        size_t gases = 2;
        size_t army_size = observation->GetUnits(Unit::Alliance::Self, IsArmy()).size();
       // std::cout << observation->GetGameLoop() << std::endl;
        if (observation->GetGameLoop() % 1000 == 0) {
            std::cout << stage << std::endl;
        }

        if ( stage < 1 && observation->GetGameLoop() > 6000 && army_size < 16) {
            stage = 1;
            std::cout << "Setting stage to 1" << std::endl;
        }
        else if (stage < 2 && observation->GetGameLoop() > 12000 && army_size > 20) {
            stage = 2;
            std::cout << "Setting stage to 2" << std::endl;
        }

        if (CountUnitType(UNIT_TYPEID::ZERG_DRONE) >= 34) {
            gases = 4;
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_DRONE) >= 16) {
            if (CountUnitType(UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
                TryBuildStructure(ABILITY_ID::BUILD_SPAWNINGPOOL);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_EXTRACTOR) < gases) {
                Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
                for (const auto& base : bases) {
                    TryBuildGas(base->pos);
                }
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_BANELINGNEST) < 1 && stage > 0) {
                TryBuildStructure(ABILITY_ID::BUILD_BANELINGNEST);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_ROACHWARREN) < 1 && stage > 0) {
                TryBuildStructure(ABILITY_ID::BUILD_ROACHWARREN);
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_LAIR) < 1 && stage > 0) {
                const Unit* hatch = nullptr;
                if (GetRandomUnit(hatch, observation, UNIT_TYPEID::ZERG_HATCHERY)) {
                    Actions()->UnitCommand(hatch, ABILITY_ID::MORPH_LAIR);
                }
            }
            else if (CountUnitType(UNIT_TYPEID::ZERG_HYDRALISKDEN) < 1 && stage > 1) {
                TryBuildStructure(ABILITY_ID::BUILD_HYDRALISKDEN);
            }
        }
        if (CountUnitType(UNIT_TYPEID::ZERG_LARVA) <= 1 && observation->GetMinerals() >= 300 && CountUnitType(UNIT_TYPEID::ZERG_HATCHERY) < 5) {
            TryBuildExpantion();
        }
        return true;
    }
    */

    bool TryBuildGas() {
        const ObservationInterface* observation = Observation();
        Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            Point2D base_location = base->pos;

            Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsVespeneGeyser());

            float minimum_distance = 50.0f;
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

            return TryBuildStructure(ABILITY_ID::BUILD_EXTRACTOR, UNIT_TYPEID::ZERG_DRONE, closestGeyser);
        }
        return false;
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

        const Unit* unit_to_build = nullptr;
        Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::ZERG_DRONE));
        for (const auto& unit : units) {

            for (const auto& order : unit->orders) {
                if (order.ability_id == ability_Type_for_structure) {
                    return false;
                }
            }
        }
        unit_to_build = GetRandomEntry(units);

        float rx = GetRandomScalar();
        float ry = GetRandomScalar();
        Units townHalls = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
        const Unit* base = GetRandomEntry(townHalls);
        Point2D buildPoint(base->pos.x + rx * 13.0f, base->pos.y + ry * 13.0f);
       // bool fug = observation->HasCreep(buildPoint);

        if (unit_to_build != nullptr) {
            Actions()->UnitCommand(unit_to_build,
                ability_Type_for_structure,
                buildPoint);

            return true;
        }
        return false;
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

            if (gas->assigned_harvesters < gas->ideal_harvesters) {
                const Unit* drone = nullptr;
                GetRandomUnit(drone, observation, UNIT_TYPEID::ZERG_DRONE);
                if (drone != nullptr) {
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

    /*
    std::vector<Unit*> GetNearEnemies(const Point2D& start, float distance) {
        return GetNearGroup(start, distance, Unit::Alliance::Enemy);
    }
    std::vector<Unit*> GetNearGroup(const Point2D& start, float distance, Unit::Alliance unit_ally = Unit::Alliance::Self) {
        Units units = Observation()->GetUnits(unit_ally);
        std::vector<Unit*> close;
        for (const auto& unit : units) {
            float unit_distance = DistanceSquared2D(unit->pos, start);
            if (unit_distance <= distance) {
                close.push_back(unit);
            }
        }
        return close;
    }
    */

    size_t UnitsInQueue(ABILITY_ID command) {
        size_t num = 0;
        for (unsigned i = 0; i < build_queue.size(); i++) {
            if (build_queue[i].GetCommand() == command) { // we need to be able to see inisde of the queue
                num++;
            }
        }
        return num;
    }

    UNIT_TYPEID GetUnitFromCommand(ABILITY_ID command) {
        switch (command) {
        case ABILITY_ID::TRAIN_DRONE:
            return UNIT_TYPEID::ZERG_DRONE;
        case ABILITY_ID::BUILD_HATCHERY:
            return UNIT_TYPEID::ZERG_HATCHERY;
        case ABILITY_ID::TRAIN_OVERLORD:
            return UNIT_TYPEID::ZERG_OVERLORD;
        case ABILITY_ID::BUILD_SPAWNINGPOOL:
            return UNIT_TYPEID::ZERG_SPAWNINGPOOL;
        default:
            std::cout << "unknown command" << std::endl;
            return UNIT_TYPEID::ZERG_LARVA;
        }
    }
};


