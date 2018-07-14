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
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }
    coordinator.SetMultithreaded(true);
    
    Party_Boi boi;
    JohnBot bot;
    
    Human james;
    
    
    std::vector<sc2::Race> races = {Race::Terran, Race::Zerg, Race::Protoss};
    std::vector<sc2::Difficulty> difficulties = {Difficulty::VeryEasy, Difficulty::Easy, Difficulty::Medium, Difficulty::MediumHard, Difficulty::Hard, Difficulty::HardVeryHard, Difficulty::VeryHard, Difficulty::CheatVision, Difficulty::CheatMoney, Difficulty::CheatMoney};
    
    
    
    
    coordinator.SetParticipants({
        //CreateParticipant(Race::Terran, &james),
        CreateParticipant(Race::Terran, &boi),
        CreateComputer(Race::Zerg, Difficulty::VeryHard)
        });

    coordinator.LaunchStarcraft();
    bool do_break = false;
    
    
    for(int i=4;i<difficulties.size() && !do_break;i++){
        for(int j=0;j<races.size() && !do_break;j++)
        {
            coordinator.StartGame(sc2::kMapBelShirVestigeLE);
            while (coordinator.Update() && !do_break){
                if (sc2::PollKeyPress()) {
                    do_break = true;
                }
            }
            //coordinator.
            coordinator.SetParticipants({
                CreateParticipant(Race::Terran, &boi),
                CreateComputer(races[j],difficulties[i])
            });
        }
    }
    
    if(!do_break){
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
    }
    while(coordinator.Update() && !do_break){
        if (sc2::PollKeyPress()) {
            do_break = true;
        }
    }
    
    boi.Control()->DumpProtoUsage();
    

    
    
    
    /*
    // Add the custom bot, it will control the players.
    sc2::ProtossMultiplayerBot bot1;
    sc2::ZergMultiplayerBot bot2;
    sc2::TerranMultiplayerBot bot3;
    
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Protoss, &bot1),
        CreateComputer(sc2::Race::Terran,sc2::Difficulty::Hard)
    });
    
     
    
     
    // Start the game.
    coordinator.LaunchStarcraft();
    bool do_break = false;
    
    for (; !do_break;) {
        coordinator.StartGame(sc2::kMapBelShirVestigeLE);
        //bot1.air_build_ = !bot1.air_build_;
        bot1.air_build_ = false;
        bot2.mutalisk_build_ = false;
        
        while (coordinator.Update() && !do_break) {
            if (sc2::PollKeyPress()) {
                do_break = true;
            }
        }
    }
    bot1.Control()->DumpProtoUsage();
    bot2.Control()->DumpProtoUsage();
    return 0;
    */

    return 0;
}
