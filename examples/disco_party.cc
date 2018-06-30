#include <sc2api/sc2_api.h>

#include <iostream>
#include "party_boi.cc"
#include "JohnBot.cc"

using namespace sc2;

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Party_Boi boi;
    Bot bot;
    
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &boi),
        //CreateParticipant(Race::Terran, &bot)
        CreateComputer(Race::Terran, Difficulty::Hard)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()){
        
    }

    return 0;
}
