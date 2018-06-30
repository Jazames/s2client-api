//{C++}
#include <sc2api/sc2_api.h>
#include <iostream>

using namespace sc2;

class Bot : public Agent {
    public:
        virtual void OnGameStart() final {
            std::cout << "Hello, World!" << std::endl;
        }
        virtual void OnStep() final {
            //std::cout << "Game loop number: " << Observation()->GetGameLoop() << "Minerals: " << Observation()->GetMinerals() << std::endl;
            TryBuildSupplyDepot();
            TryBuildRefinery();
        }
        virtual void OnUnitCreated(const Unit* unit) final {
            //std::cout << "Unit created: " << unit->unit_type.to_string() << std::endl;
            switch(unit->unit_type.ToType())
            {
                case UNIT_TYPEID::TERRAN_REFINERY:
                    //Get three doods to mine gas.
                    for(int i=0;i<3;i++)
                    {
                        auto dood = GetBuilder();
                        Actions()->UnitCommand(dood, ABILITY_ID::SMART, unit);
                    }
                    break;
                default:
                    break;
            }
        }
        virtual void OnUnitIdle(const Unit* unit) final {
            switch(unit->unit_type.ToType())
            {
                case UNIT_TYPEID::TERRAN_COMMANDCENTER:{
                    Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
                    break;
                }
                case UNIT_TYPEID::TERRAN_SCV: {
                    const Unit* mineral_target = FindNearestMineralPatch(unit->pos);
                    if(!mineral_target){
                        break;
                    }
                    Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
                    break;
                }
                default:
                    break;
            }
        }
    private:
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
            if(Observation()->GetMinerals() < 100){
                return false;
            }
            
            //If there are already 2 refinersy, don't make more.
            int count = 0;
            auto units = Observation()->GetUnits(Unit::Alliance::Self);
            for(auto unit : units)
            {
                if(unit->unit_type == UNIT_TYPEID::TERRAN_REFINERY){
                    count++;
                }
            }
            
            //std::cout << "count: " << count << std::endl;
            if(count > 1)
            {
                return false;
            }
            
            auto builder = GetBuilder();
            auto geyser = FindNearestVespeneGyser(builder->pos);
            return TryBuildStructure(ABILITY_ID::BUILD_REFINERY, builder, geyser);
        }
        bool TryBuildSupplyDepot(){
            const ObservationInterface* observation = Observation();
            
            //Don't build a supply depot if not supply capped
            if(observation->GetFoodUsed() < observation->GetFoodCap() - 2){
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
            const Unit* unit_to_build = nullptr;
            UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV;
            
            Units units = Observation()->GetUnits(Unit::Alliance::Self);
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
                    unit_to_build = unit;
                }
            }
            return unit_to_build;
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
        
};


int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);
    
    Bot bot;
    coordinator.SetParticipants({CreateParticipant(Race::Terran, &bot), CreateComputer(Race::Zerg)});
    
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);
    
    
    while(coordinator.Update());
    return 0;
}
