//{C++}
#include <sc2api/sc2_api.h>
#include "sc2lib/sc2_lib.h"
#include <iostream>

using namespace sc2;

class Party_Boi : public Agent {
private:
    std::vector<Point3D> m_expansions;
    bool m_expanding = false;
    bool m_start_saving_for_expand = false;
public:
    virtual void OnGameStart() final {
        //Need to reset all variables, for when the ai is immediate loaded into consecutive games.
        m_expanding = false;
        m_start_saving_for_expand = false;
        
        std::cout << "I'm a Turtle." << std::endl;
        auto doods = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER));
        for(auto dood : doods){
            Actions()->UnitCommand(dood, ABILITY_ID::RALLY_COMMANDCENTER, dood->pos);
        }
        m_expansions = search::CalculateExpansionLocations(Observation(), Query());
    }
    virtual void OnStep() final {
        //std::cout << "Game loop number: " << Observation()->GetGameLoop() << "Minerals: " << Observation()->GetMinerals() << std::endl;
        TryBuildSupplyDepot();
        TryBuildRefinery();
        TryBuildBarracks();
        TryMassAttack();
        TryExpand();
        TryBuildEngineeringBay();
        TryBuildFactory();
        TryBuildArmory();
        TryBuildStarport();
        
        //Now try telling everything that idleness is sin.
        auto lazy_doodz = Observation()->GetUnits(Unit::Alliance::Self, IsIdle());
        for(auto laze : lazy_doodz){
            thouShaltNotBeIdle(laze);
        }
    }
    virtual void OnError(const std::vector<ClientError>& error/*client_errors*/, const std::vector<std::string>& error_strings/*protocol_errors*/ = {}) final{
        for(auto error_string : error_strings){
            std::cout << "ERROR: " << error_string << std::endl;
        }
    }
        
    virtual void OnUnitIdle(const Unit* unit) final{
            //Is taken care of in OnStep.
        }
        virtual void OnUnitCreated(const Unit* unit) final {
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
                    m_expanding = false;
                    break;
                default:
                    break;
            }
        }
        virtual void OnUnitDestroyed(const Unit* unit)
        {
            //Only care if it's a friendly killed.
            if(unit->alliance != Unit::Alliance::Self)
            {
                return;
            }
            //Trash Talk
            if(IsTownHall()(*unit))
            {
                Actions()->SendChat("That was Gay!");
            }
            
            /*
            if(unit->unit_type.ToType() == UNIT_TYPEID::TERRAN_SCV && Observation()->GetFoodArmy() < 30 && Observation()->GetUnits(Unit::Alliance::Enemy).size() < 20)
            {
                auto frenz = Observation()->GetUnits(Unit::Alliance::Self);
                Actions()->UnitCommand(frenz, ABILITY_ID::ATTACK_ATTACK, unit->pos, false);
            }
            */
            ConsiderAttack(unit->pos);
            
 
        }
        virtual void OnUnitEnterVision(const Unit* unit)
        {
            ConsiderAttack(unit->pos);
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
        
        bool HaveSufficientMaterials(UNIT_TYPEID unit_type)
        {
            auto data = Observation()->GetUnitTypeData();
            auto mineral_cost = data[(int)unit_type].mineral_cost;
            auto vespene_cost = data[(int)unit_type].vespene_cost;
            auto food_cost = data[(int)unit_type].food_required;
            
            return Observation()->GetMinerals() >= mineral_cost && Observation()->GetVespene() >= vespene_cost && (Observation()->GetFoodCap() - Observation()->GetFoodUsed()) >= food_cost;
        }
        
        Units GetUnitsInArea(Unit::Alliance alliance, Point2D pos, float radius){
            Units doodz;
            auto peepz = Observation()->GetUnits(alliance);
            for(auto peep : peepz)
            {
                auto distance_x = pos.x - peep->pos.x;
                auto distance_y = pos.y - peep->pos.y;
                auto distance = std::sqrt(distance_x * distance_x + distance_y * distance_y);
                if(distance <= radius)
                {
                    doodz.push_back(peep);
                }
            }
            return doodz;
        }
        
        bool GroupContainsUnitType(UNIT_TYPEID unit_type, Units units)
        {
            for(auto dood : units)
            {
                if(unit_type == dood->unit_type)
                {
                    return true;
                }
            }
            return false;
        }
        
        bool TryBuildAddon(const Unit* unit_to_build, ABILITY_ID ability_id_for_build_type)
        {
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_TECHLAB) && !HaveSufficientMaterials(UNIT_TYPEID::TERRAN_REACTOR)){
                return false;
            }
            
            if(unit_to_build->build_progress < 1.0){
                return false;
            }
            
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            return TryBuildStructure(ability_id_for_build_type,
                                     unit_to_build,
                                     Point2D(unit_to_build->pos.x + rx * 15.0f, unit_to_build->pos.y + ry * 15.0f)
                                     );
        }
        bool TryBuildRefinery(){
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_REFINERY)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            
            //Don't build gas before a barracks.
            if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < 1){
                return false;
            }
            
            //No need to get gas if there is more gas than minerals.
            if(Observation()->GetVespene() > Observation()->GetMinerals()){
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
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_SUPPLYDEPOT)){
                return false;
            }
            if(m_start_saving_for_expand){
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
            bool build_in_parallel = observation->GetFoodUsed() >= observation->GetFoodCap();
            
            return TryBuildStructure(ABILITY_ID::BUILD_SUPPLYDEPOT,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f),
                                     build_in_parallel
                                     );
        }
        
        bool TryExpand() {
            auto townhalls = CountUnitType(Observation(), IsTownHall());
            
            if(!((Observation()->GetFoodUsed() > townhalls * 30 && townhalls < 2) || (Observation()->GetFoodUsed() > townhalls * 40) || Observation()->GetMinerals() > 1000))
            {
                m_start_saving_for_expand = false;
                return false;
            }
            
            m_start_saving_for_expand = true;
            
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_COMMANDCENTER)){
                return false;
            }
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            for (const auto& unit : units) {
                for (const auto& order : unit->orders) {
                    if (order.ability_id == ABILITY_ID::BUILD_COMMANDCENTER) {
                        m_start_saving_for_expand = false;
                        return false;
                    }
                }
            }
            
            //const ObservationInterface* observation = Observation();
            float minimum_distance = std::numeric_limits<float>::max();
            auto start_location = Observation()->GetStartLocation();
            //auto staging_location = start_location;
            
            Point3D closest_expansion;
            auto expansions = m_expansions;
            
            //Don't try to expand if everything is taken.
            if(Observation()->GetUnits(IsTownHall()).size() >= expansions.size()){
                return false;
            }
            
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
            m_expanding = TryBuildStructure(ABILITY_ID::BUILD_COMMANDCENTER, builder, closest_expansion);
            m_start_saving_for_expand = !m_expanding;
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
        bool TryBuildFortress(const Unit* command_center)
        {
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_PLANETARYFORTRESS)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if(command_center->unit_type.ToType() != UNIT_TYPEID::TERRAN_COMMANDCENTER)
            {
                return false;
            }
            
            Actions()->UnitCommand(command_center, ABILITY_ID::MORPH_PLANETARYFORTRESS);
            
            
            return true;
        }
        
        bool TryBuildBarracks()
        {
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_BARRACKS)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if(!(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < 5))
            {
                return false;
            }
            if(!(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < Observation()->GetFoodCap()/16))
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
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_ENGINEERINGBAY)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < 1)){
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
        bool TryBuildFactory(){
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_FACTORY)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if(Observation()->GetFoodArmy() < 30){
                return false;
            }
            if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS) < 2){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_FACTORY) * 3 > CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS)))
            {
                return false;
            }
            
            //Try to make a barracks
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_FACTORY,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        bool TryBuildArmory(){
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_ARMORY)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_FACTORY) < 1)){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_ARMORY) > 0))
            {
                return false;
            }
            
            //Try to make a barracks
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_ARMORY,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        bool TryBuildStarport(){
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_STARPORT)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_ARMORY) < 1)){
                return false;
            }
            if((CountUnitType(Observation(), UNIT_TYPEID::TERRAN_STARPORT) * 3 > CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKS)))
            {
                return false;
            }
            
            //Try to make a barracks
            float rx = GetRandomScalar();
            float ry = GetRandomScalar();
            
            auto builder = GetBuilder();
            
            return TryBuildStructure(ABILITY_ID::BUILD_STARPORT,
                                     builder,
                                     Point2D(builder->pos.x + rx * 15.0f, builder->pos.y + ry * 15.0f)
                                     );
        }
        
        bool ConsiderAttack(Point2D pos){
            auto threat = GetUnitsInArea(Unit::Alliance::Enemy, pos, 10.0);
            
            //If they have many more doodz, don't do it.
            auto num_baddies = Observation()->GetUnits(Unit::Alliance::Enemy, IsArmy(Observation())).size();
            
            
            auto doodz = Observation()->GetUnits(Unit::Alliance::Self, IsArmy(Observation()));
            auto army_size = doodz.size();
            
            //Retreat like a pansy if they've got you outnumberd pretty bad.
            if(num_baddies * 0.8 > army_size){
                Actions()->UnitCommand(doodz, ABILITY_ID::MOVE, Observation()->GetStartLocation());
                return false;
            }
            
            /*
            if(GroupContainsUnitType(UNIT_TYPEID::TERRAN_SIEGETANK, threat)){
                Actions()->UnitCommand(doodz, ABILITY_ID::MOVE, pos);
            }
             */
            
            
            
            
            bool queue_order = threat.size() < 5 ;
            Actions()->UnitCommand(doodz, ABILITY_ID::SCAN_MOVE, pos, queue_order);
            Actions()->UnitCommand(doodz, ABILITY_ID::ATTACK_ATTACK, pos, queue_order);
            
            //Do a stim
            auto medivacs = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC));
            if(num_baddies >= 0.5 * army_size && medivacs.size() > 0){
                std::cout << "doing stim packs" << std::endl;
                Actions()->UnitCommand(doodz, ABILITY_ID::EFFECT_STIM);
            }
            
            //If there are more medivacs than doodz, then have the medivacs run away.
            //auto medivacs = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC));
            if(medivacs.size() * 2 > army_size){
                Actions()->UnitCommand(medivacs, ABILITY_ID::MOVE, Observation()->GetStartLocation());
            }
            return true;
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
            Actions()->UnitCommand(doodz, ABILITY_ID::SCAN_MOVE, target);
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
            if(m_start_saving_for_expand){
                return false;
            }
            if(barracks->unit_type != UNIT_TYPEID::TERRAN_BARRACKS){
                return false;
            }
            if(barracks->add_on_tag && Observation()->GetUnit(barracks->add_on_tag)->unit_type == UNIT_TYPEID::TERRAN_BARRACKSTECHLAB && HaveSufficientMaterials(UNIT_TYPEID::TERRAN_MARAUDER)){
                
                for (const auto& order : barracks->orders) {
                    if (order.ability_id == ABILITY_ID::TRAIN_MARAUDER) {
                        return false;
                    }
                }
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARAUDER);
            }
            else if(barracks->add_on_tag && HaveSufficientMaterials(UNIT_TYPEID::TERRAN_MARINE)){
                int i = 0;
                for (const auto& order : barracks->orders) {
                    if (order.ability_id == ABILITY_ID::TRAIN_MARINE) {
                        i++;
                    }
                    if(i>1){
                        return false;
                    }
                }
                Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
                //Make a second marine if there's enough money.
                if(HaveSufficientMaterials(UNIT_TYPEID::TERRAN_MARINE)){
                    Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
                }
            }
            else{
                
                for (const auto& order : barracks->orders) {
                    if (order.ability_id == ABILITY_ID::TRAIN_MARINE) {
                        return false;
                    }
                }
                //Actions()->UnitCommand(barracks, ABILITY_ID::TRAIN_MARINE);
            }
            return true;
        }
        
        bool TryBuildMedivac(const Unit* starport)
        {
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_MEDIVAC)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if(starport->unit_type != UNIT_TYPEID::TERRAN_STARPORT){
                return false;
            }
            for (const auto& order : starport->orders) {
                if (order.ability_id == ABILITY_ID::TRAIN_MEDIVAC) {
                    return false;
                }
            }
            Actions()->UnitCommand(starport, ABILITY_ID::TRAIN_MEDIVAC);
            
            return true;
        }
        bool TryBuildHellbat(const Unit* factory)
        {
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_HELLION)){
                return false;
            }
            if(m_start_saving_for_expand){
                return false;
            }
            if(factory->unit_type != UNIT_TYPEID::TERRAN_FACTORY){
                return false;
            }
            for (const auto& order : factory->orders) {
                if (order.ability_id == ABILITY_ID::TRAIN_HELLBAT) {
                    return false;
                }
            }
            Actions()->UnitCommand(factory, ABILITY_ID::TRAIN_HELLBAT);
            return true;
        }
        
        bool TryBuildSCV(const Unit* command_center){
            
            if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_SCV)){
                return false;
            }
            
            //If the unit is not a town hall, quit now!
            //Hopefully this function is never given a non-town hall, but you never know.
            if(!IsTownHall()(*command_center)){
                return false;
            }
            
            //We don't need too many doodz.
            if(Observation()->GetFoodWorkers() > 70){
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
            
            for (const auto& order : command_center->orders) {
                if (order.ability_id == ABILITY_ID::TRAIN_SCV) {
                    return false;
                }
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
        
        size_t CountUnitType(const ObservationInterface* observation, Filter filter) {
            return observation->GetUnits(Unit::Alliance::Self, filter).size();
        }
        
        
        void thouShaltNotBeIdle(const Unit* unit) {
            //std::cout << "Unit order size: " << unit->orders.size() << std::endl;
            switch(unit->unit_type.ToType())
            {
                case UNIT_TYPEID::TERRAN_COMMANDCENTER:
                    TryBuildFortress(unit);
                case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
                case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
                    TryBuildSCV(unit);
                    break;
                case UNIT_TYPEID::TERRAN_SCV: {
                    MineIdleWorkers(unit, ABILITY_ID::HARVEST_GATHER_SCV);
                    break;
                }
                case UNIT_TYPEID::TERRAN_BARRACKSFLYING:
                case UNIT_TYPEID::TERRAN_BARRACKS:{
                    //std::cout << Observation()->GetUnit(unit->add_on_tag) << std::endl;
                    if(!HaveSufficientMaterials(UNIT_TYPEID::TERRAN_REACTOR)){
                        TryBuildSoldier(unit);
                    }
                    else if(Observation()->GetUnit(unit->add_on_tag)){
                        TryBuildSoldier(unit);
                    }
                    else if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKSREACTOR) > CountUnitType(Observation(), UNIT_TYPEID::TERRAN_BARRACKSTECHLAB)){
                        TryBuildAddon(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
                    }
                    else {
                        TryBuildAddon(unit, ABILITY_ID::BUILD_REACTOR_BARRACKS);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR);
                    break;
                case UNIT_TYPEID::TERRAN_ARMORY:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONS);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATING);
                    //Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONS);
                    break;
                case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_COMBATSHIELD);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_CONCUSSIVESHELLS);
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_STIMPACK);
                    break;
                case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
                    Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_INFERNALPREIGNITER);
                    break;
                case UNIT_TYPEID::TERRAN_FACTORYFLYING:
                case UNIT_TYPEID::TERRAN_FACTORY:{
                    //std::cout << Observation()->GetUnit(unit->add_on_tag) << std::endl;
                    if(Observation()->GetUnit(unit->add_on_tag)){//->unit_type == UNIT_TYPEID::TERRAN_REACTOR){
                        TryBuildHellbat(unit);
                    }
                    else if(CountUnitType(Observation(), UNIT_TYPEID::TERRAN_TECHLAB) < 1){
                        TryBuildAddon(unit, ABILITY_ID::BUILD_TECHLAB_FACTORY);
                    }
                    else {
                        TryBuildAddon(unit, ABILITY_ID::BUILD_REACTOR_FACTORY);
                    }
                    break;
                }
                case UNIT_TYPEID::TERRAN_STARPORT:
                    TryBuildMedivac(unit);
                    break;
                case UNIT_TYPEID::TERRAN_MARINE:{
                    
                    //Don't be ambitious until food cap reached.
                    if(Observation()->GetFoodUsed() < 200){
                        break;
                    }
                    
                    auto baddies = Observation()->GetUnits(Unit::Alliance::Enemy);
                    if(baddies.size() > 0){
                        ConsiderAttack(baddies[0]->pos);
                    }
                    
                    //Tell a marine to scout attack a random expansion. It should be interesting.
                    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, GetRandomEntry(m_expansions));
                    break;
                }
                default:
                    break;
            }
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
                /*
                 for(int i=0; i < geyser->assigned_harvesters - geyser->ideal_harvesters; i++)
                 {
                 auto dood = FindNearestWorker(geyser->pos);
                 OnUnitIdle(dood);
                 }
                 */
                
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
                /*
                 for(int i=0; i < base->assigned_harvesters - base->ideal_harvesters; i++)
                 {
                 auto dood = FindNearestWorker(base->pos);
                 OnUnitIdle(dood);
                 }
                 */
            }
            
            if (!worker->orders.empty()) {
                return;
            }
            
            //If all workers are spots are filled just go to any base.
            const Unit* random_base = GetRandomEntry(bases);
            valid_mineral_patch = FindNearestMineralPatch(random_base->pos);
            Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
        }
        const Unit* FindNearestWorker(const Point2D start){
            Units units = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
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
        
        bool TryBuildStructure(ABILITY_ID ability_type_for_structure, const Unit* unit_to_build, Point2D position, bool build_in_parallel = false){
            //const ObservationInterface* observation = Observation();
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            if(!build_in_parallel){
                for (const auto& unit : units) {
                    for (const auto& order : unit->orders) {
                        if (order.ability_id == ability_type_for_structure) {
                            return false;
                        }
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
            //std::string building_name = Observation()->Get
            //Observation()->IsPlacable(position) ? std::cout << "Can't build structure: " << building_name <<std::endl: std::cout << "It's cool" << std::endl;
            return true;
        }
        bool TryBuildStructure(ABILITY_ID ability_type_for_structure, const Unit* unit_to_build, const Unit* position, bool build_in_parallel = false){
            //const ObservationInterface* observation = Observation();
            
            // if a unit is already building a structure of this type, do nothing.
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
            if(!build_in_parallel){
                for (const auto& unit : units) {
                    for (const auto& order : unit->orders) {
                        if (order.ability_id == ability_type_for_structure) {
                            return false;
                        }
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
