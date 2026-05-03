#pragma once
#include <Rubiks.hpp>
#include <Skewb.hpp>

std::pair<bool, std::vector<Move>> Start_IDDFS(const Rubiks &initial_state);
std::pair<bool, std::vector<Move>> Start_phase2_IDA_Star(const Rubiks &phase1_cube);
std::pair<bool, std::vector<Move>> Start_phase1_IDA_Star(const Rubiks &initial_state);
std::pair<bool, std::vector<Move>> Start_skewb_IDA_Star(const Skewb &initial_state);
