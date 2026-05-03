#pragma once
#include <Rubiks.hpp>

const Move &get_move(int move_nb);
const Move &get_move_restricted(int move_nb);
const Move &get_skewb_move(int move_nb);

Face opposite_face(Face face);
bool should_search_with_move(const Move& next_mone, const Move& prev_move);

std::vector<Move> parse_moves_classic(const std::string& moves_str);
std::vector<Move> parse_moves_skewb(const std::string& moves_str);
