#include <sc2api/sc2_api.h>
#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include "party_boi.cc"
#include "JohnBot.cc"

using namespace sc2;

class Human : public sc2::Agent {
public:
    void OnGameStart() final {
        Debug()->DebugTextOut("Human");
        Debug()->SendDebug();
    }
};

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Party_Boi boi;
    JohnBot bot;
    
    Human james;
    
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &james),
        CreateParticipant(Race::Terran, &boi)
        //CreateComputer(Race::Terran, Difficulty::VeryHard)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()){
        
    }

    return 0;
}
