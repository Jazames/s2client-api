#include <sc2api/sc2_api.h>

#include <iostream>

#include "sc2utils/sc2_manage_process.h"

#include "party_boi.cc"
#include "JohnBot.cc"

using namespace sc2;

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Party_Boi boi;
    JohnBot bot;
    
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &boi),
        CreateParticipant(Race::Zerg, &bot)
        //CreateComputer(Race::Random, Difficulty::Medium)
        });

    coordinator.LaunchStarcraft();
   // coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    bool do_break = false;
    while (!do_break) {
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }

    return 0;
}
