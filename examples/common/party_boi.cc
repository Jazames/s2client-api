//{C++}
#include <sc2api/sc2_api.h>
#include "sc2lib/sc2_lib.h"
#include <iostream>

using namespace sc2;

class Party_Boi : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
        auto doods = Observation()->GetUnits(Unit::Alliance::Self);
        for(auto dood : doods){
            if(dood->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER)
            {
                Actions()->UnitCommand(dood, ABILITY_ID::RALLY_COMMANDCENTER, dood->pos);
            }
        }
        }
        virtual void OnStep() final {
            //std::cout << "Game loop number: " << Observation()->GetGameLoop() << "Minerals: " << Observation()->GetMinerals() << std::endl;
            TryBuildSupplyDepot();
            TryBuildRefinery();
            TryBuildBarracks();
            TryMassAttack();
            TryExpand();
            TryBuildEngineeringBay();
        }
        virtual void OnUnitCreated(const Unit* unit) final {
            //Now try telling all Idle buildings to start working again.
            auto lazy_doodz = Observation()->GetUnits(Unit::Alliance::Self, IsIdle());
            for(auto laze : lazy_doodz){
                OnUnitIdle(laze);
            }
            
            //std::cout << "Unit created: " << unit->unit_type.to_string() << std::endl;
            switch(unit->unit_type.ToType())
            {
                case UNIT_TYPEID::TERRAN_REFINERY:
                    /*
                    //Get three doods to mine gas.
                    for(int i=0;i<3;i++)
                    {
                        auto dood = GetBuilder();
                        Actions()->UnitCommand(dood, ABILITY_ID::SMART, unit);
                    }
                    */
                    break;
                case UNIT_TYPEID::TERRAN_COMMANDCENTER:
                    expanding = false;
                    break;
                default:
                    break;
            }
        }
        virtual void OnUnitIdle(const Unit* unit) final {
            //std::cout << "Unit order size: " << unit->orders.size() << std::endl;
            switch(unit->unit_type.ToType())
            {
                case UNIT_TYPEID::TERRAN_COMMANDCENTER:{
                    //Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                    TryBuildSCV(unit);
                    break;
                }
                case UNIT_TYPEID::TERRAN_SCV: {
                    MineIdleWorkers(unit, ABILITY_ID::HARVEST_GATHER_SCV);
                    break;
                }
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:
                case UNIT_TYPEID::TERRAN_BARRACKS:{
                    //std::cout << Observation()->GetUnit(unit->add_on_tag) << std::endl;
                    if(Observation()->GetUnit(unit->add_on_tag)){//->unit_type == UNIT_TYPEID::TERRAN_REACTOR){
                        TryBuildSoldier(unit);
                        //TryBuildSoldier(unit);
                    }
                    else if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKSREACTOR) < CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKSTECHLAB)){
                        TryBuildAddon(unit, ABILITY_ID::BUILD_REACTOR_BARRACKS);
                    }
                    else {
                        TryBuildAddon(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR);
                    break;
                case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_COMBATSHIELD);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_CONCUSSIVESHELLS);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_STIMPACK);
                default:
                    break;
            }
        }
        virtual void OnUnitEnterVision(const Unit* unit)
        {
            auto doodz = Observation()->GetUnits(Unit::Alliance::Self, IsArmy(Observation()));
            bool queue_order = (Observation()->GetUnits(Unit::Alliance::Enemy).size() > 4);
            Actions()->UnitCommand(doodz, ABILITY_ID::ATTACK_ATTACK, unit->pos, queue_order);
        }
    private:
        struct IsTownHall {
            bool operator()(const Unit& unit) {
                switch (unit.unit_type.ToType()) {
                    case UNIT_TYPEID::ZERG_HATCHERY: return true;
                    case UNIT_TYPEID::ZERG_LAIR: return true;
                    case UNIT_TYPEID::ZERG_HIVE : return true;
                    case UNIT_TYPEID::TERRAN_COMMANDCENTER: return true;
                    case UNIT_TYPEID::TERRAN_ORBITALCOMMAND: return true;
                    case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING: return true;
                    case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS: return true;
                    case UNIT_TYPEID::PROTOSS_NEXUS: return true;
                    default: return false;
                }
            }
        };
        
        //Ignores Overlords, workers, and structures
        struct IsArmy {
            IsArmy(const ObservationInterface* obs) : observation_(obs) {}
            
            bool operator()(const Unit& unit) {
                auto attributes = observation_->GetUnitTypeData().at(unit.unit_type).attributes;
                for (const auto& attribute : attributes) {
                    if (attribute == Attribute::Structure) {
                        return false;
                    }
                }
                switch (unit.unit_type.ToType()) {
                    case UNIT_TYPEID::ZERG_OVERLORD: return false;
                    case UNIT_TYPEID::PROTOSS_PROBE: return false;
                    case UNIT_TYPEID::ZERG_DRONE: return false;
                    case UNIT_TYPEID::TERRAN_SCV: return false;
                    case UNIT_TYPEID::ZERG_QUEEN: return false;
                    case UNIT_TYPEID::ZERG_LARVA: return false;
                    case UNIT_TYPEID::ZERG_EGG: return false;
                    case UNIT_TYPEID::TERRAN_MULE: return false;
                    case UNIT_TYPEID::TERRAN_NUKE: return false;
                    default: return true;
                }
            }
            const ObservationInterface* observation_;
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
        
        struct IsIdle {
            bool operator()(const Unit& unit) {
                return unit.orders.size() == 0;
            }
        };
        
        bool TryBuildAddon(const Unit* unit_to_build, ABILITY_ID ability_id_for_build_type)
        {
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            return TryBuildStructure(ability_id_for_build_type,
                                     unit_to_build,
                                     Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f)
                                     );
        }
        
        bool TryExpand() {
            auto townhalls = CountUnitType(Observation(), UNIT_TYPEID::TERRAN_COMMANDCENTER) +
            CountUnitType(Observation(), UNIT_TYPEID::TERRAN_ORBITALCOMMAND) +
            CountUnitType(Observation(), UNIT_TYPEID::TERRAN_PLANETARYFORTRESS);
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            for (const auto& unit : units) {
                for (const auto& order : unit->orders) {
                    if (order.ability_id == ABILITY_ID::BUILD_COMMANDCENTER) {
                        start_saving_for_expand = false;
                        return false;
                    }
                }
            }
            
            
            if(Observation()->GetFoodWorkers() > 66)
            {
                return false;
            }
            if(Observation()->GetFoodUsed() < townhalls * 40)
            {
                
                return false;
            }
            
            start_saving_for_expand = true;
            
            if(Observation()->GetMinerals() < 500)
            {
                return false;
            }
            
            //const ObservationInterface* observation = Observation();
            float minimum_distance = std::numeric_limits<float>::max();
            auto start_location = Observation()->GetStartLocation();
            //auto staging_location = start_location;
            
            Point3D closest_expansion;
            auto expansions = search::CalculateExpansionLocations(Observation(), Query());
            for (const auto& expansion : expansions) {
                float current_distance = Distance2D(start_location, expansion);
                if (current_distance < .01f) {
                    continue;
                }
                
                if (current_distance < minimum_distance) {
                    if (Query()->Placement(ABILITY_ID::BUILD_COMMANDCENTER, expansion)) {
                        closest_expansion = expansion;
                        minimum_distance = current_distance;
                    }
                }
            }
            auto builder = GetBuilder();
            expanding = TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER, builder, closest_expansion);
            return true;
            /*
             //only update staging location up till 3 bases.
             if (TryBuildStructure(build_ability, worker_type, closest_expansion, true) && observation->GetUnits(Unit::Self, IsTownHall()).size() < 4) {
             staging_location = Point3D(((staging_location.x + closest_expansion.x) / 2), ((staging_location.y + closest_expansion.y) / 2),
             ((staging_location.z + closest_expansion.z) / 2));
             return true;
             }
             return false;
             */
        }
        
        bool TryBuildBarracks()
        {
            if(start_saving_for_expand){
                return false;
            }
            if(!(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < Observation()->GetFoodCap()/10))
            {
                return false;
            }
            
            //Try to make a barracks
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_BARRACKS,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        
        bool TryBuildEngineeringBay()
        {
            if(start_saving_for_expand){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_ENGINEERINGBAY) > 0))
            {
                return false;
            }
            
            //Try to make a barracks
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_ENGINEERINGBAY,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        
        bool TryMassAttack()
        {
            static bool order_given = false;
            
            if(Observation()->GetFoodUsed() < 200)
            {
                return false;
            }
            
            if(order_given)
            {
                return false;
            }
            Point2D target;
            FindEnemyPosition(target);
            auto doodz = Observation()->GetUnits(Unit::Alliance::Self, IsArmy(Observation()));
            Actions()->UnitCommand(doodz, ABILITY_ID::ATTACK_ATTACK, target);
        
            order_given = true;
            return true;
        }
        
        // Tries to find a random location that can be pathed to on the map.
        // Returns 'true' if a new, random location has been found that is pathable by the unit.
        bool FindEnemyPosition(Point2D& target_pos) {
            auto game_info = Observation()->GetGameInfo();
            
            if (game_info.enemy_start_locations.empty()) {
                return false;
            }
            target_pos = game_info.enemy_start_locations.front();
            return true;
        }
        
        bool TryBuildSoldier(const Unit* barracks)
        {
            if(start_saving_for_expand){
                return false;
            }
            if(barracks->unit_type != UNIT_TYPEID::TERRAN_BARRACKS){
                return false;
            }
            if(barracks->add_on_tag && Observation()->GetUnit(barracks->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSTECHLAB){
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARAUDER);
            }
            else if(barracks->add_on_tag){
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
            }
            else{
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
            }
            return true;
        }
        
        bool TryBuildSCV(const Unit* command_center){
            
            //If the unit is not a town hall, quit now!
            //Hopefully this function is never given a non-town hall, but you never know.
            if(!IsTownHall()(*command_center)){
                return false;
            }
            
            const ObservationInterface* observation = Observation();
            Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
            Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
            
            const Unit* valid_mineral_patch = nullptr;
            
            if (bases.empty()) {
                //TODO: Try to expand
                return false;
            }
            
            for (const auto& geyser : geysers) {
                if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
                    Actions()->UnitCommand(command_center, ABILITY_ID::TRAIN_SCV);
                    return true;
                }
            }
            //Search for a base that is missing workers.
            for (const auto& base : bases) {
                //If we have already mined out here skip the base.
                if (base->ideal_harvesters == 0 || base->build_progress != 1) {
                    continue;
                }
                if (base->assigned_harvesters < base->ideal_harvesters) {
                    valid_mineral_patch = FindNearestMineralPatch(base->pos);
                    Actions()->UnitCommand(command_center, ABILITY_ID::TRAIN_SCV);
                    return true;
                }
            }
            return false;
        }
        
        size_t CountUnitType(const ObservationInterface* observation, UnitTypeID unit_type) {
            return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
        }
        
        // Mine the nearest mineral to Town hall.
        // If we don't do this, probes may mine from other patches if they stray too far from the base after building.
        void MineIdleWorkers(const Unit* worker, AbilityID worker_gather_command) {
            const ObservationInterface* observation = Observation();
            Units bases = observation->GetUnits(Unit::Alliance::Self, IsTownHall());
            Units geysers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
            
            const Unit* valid_mineral_patch = nullptr;
            
            if (bases.empty()) {
                return;
            }
            
            for (const auto& geyser : geysers) {
                if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
                    Actions()->UnitCommand(worker, worker_gather_command, geyser);
                    return;
                }
            }
            //Search for a base that is missing workers.
            for (const auto& base : bases) {
                //If we have already mined out here skip the base.
                if (base->ideal_harvesters == 0 || base->build_progress != 1) {
                    continue;
                }
                if (base->assigned_harvesters < base->ideal_harvesters) {
                    valid_mineral_patch = FindNearestMineralPatch(base->pos);
                    Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
                    return;
                }
            }
            
            if (!worker->orders.empty()) {
                return;
            }
            
            //If all workers are spots are filled just go to any base.
            const Unit* random_base = GetRandomEntry(bases);
            valid_mineral_patch = FindNearestMineralPatch(random_base->pos);
            Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
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
        
        const Unit* FindNearestVespeneGyser(const Point2D& start){
            Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
            float distance = std::numeric_limits<float>::max();
            const Unit* target = nullptr;
            for (const auto& u : units) {
                if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
                    float d = DistanceSquared2D(u->pos, start);
                    if (d < distance) {
                        distance = d;
                        target = u;
                    }
                }
            }
            //std::cout << "Found gyser at " << target->pos.x << " " << target->pos.y << std::endl;
            return target;
        }
        
        bool TryBuildRefinery(){
            if(start_saving_for_expand){
                return false;
            }
            if(Observation()->GetMinerals() < 100){
                return false;
            }
            
            //Each town hall should have two refineries. No more.
            if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_REFINERY) >= 2 * Observation()->GetUnits(Unit::Alliance::Self, IsTownHall()).size())
            {
                return false;
            }
            
            auto builder = GetBuilder();
            auto geyser = FindNearestVespeneGyser(builder->pos);
            return TryBuildStructure(ABILITY_ID::BUILD_REFINERY, builder, geyser);
        }
        bool TryBuildSupplyDepot(){
            if(start_saving_for_expand){
                return false;
            }
            const ObservationInterface* observation = Observation();
            
            //Don't build a supply depot if at limit.
            if(observation->GetFoodCap() == 200)
            {
                return false;
            }
            //Don't build a supply depot unless we're over 3/4 of the food cap
            if(observation->GetFoodUsed() < observation->GetFoodCap() * 0.75){
                return false;
            }
            
            //Try to make a supply depot
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        const Unit* GetBuilder(){
            //const Unit* unit_to_build = nullptr;
            UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV;
            
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            Units workers;
            for( const auto unit : units){
                if(unit->unit_type == unit_type){
                    /*
                     bool acceptable = true;
                     for(const auto& order : unit->orders)
                     {
                     if(order.ability_id == ABILITY_ID::HARVEST_GATHER && order. == UNIT_TYPEID::TERRAN_REFINERY){
                     acceptable = false;
                     }
                     }
                     if(acceptable){
                     unit_to_build = unit;
                     }
                     */
                    workers.push_back(unit);
                    //unit_to_build = unit;
                }
            }
            return GetRandomEntry(workers);
        }
        
        bool TryBuildStructure(ABILITY_ID ability_type_for_structure, const Unit* unit_to_build, Point2D position){
            //const ObservationInterface* observation = Observation();
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            for (const auto& unit : units) {
                for (const auto& order : unit->orders) {
                    if (order.ability_id == ability_type_for_structure) {
                        return false;
                    }
                }
            }
            
            if(!unit_to_build){
                return false;
            }
            
            
            Actions()->UnitCommand(unit_to_build,
                                   ability_type_for_structure,
                                   position
                                   );
            return true;
        }
        bool TryBuildStructure(ABILITY_ID ability_type_for_structure, const Unit* unit_to_build, const Unit* position){
            //const ObservationInterface* observation = Observation();
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            for (const auto& unit : units) {
                for (const auto& order : unit->orders) {
                    if (order.ability_id == ability_type_for_structure) {
                        return false;
                    }
                }
            }
            
            if(!unit_to_build){
                return false;
            }
            
            
            Actions()->UnitCommand(unit_to_build,
                                   ability_type_for_structure,
                                   position
                                   );
            return true;
        }
        bool expanding = false;
        bool start_saving_for_expand = false;
};
        
  /*
        int main(int argc, char* argv[]) {
            Coordinator coordinator;
            coordinator.LoadSettings(argc, argv);
            
            Party_Boi bot;
            coordinator.SetParticipants({CreateParticipant(Race::Terran, &bot), CreateComputer(Race::Protoss, Difficulty::MediumHard)});
            coordinator.LaunchStarcraft();
            coordinator.StartGame(sc2::kMapBelShirVestigeLE);
            
            while(coordinator.Update());
            
            
            while(coordinator.Update());
            return 0;
        }
   */
