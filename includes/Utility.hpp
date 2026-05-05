#pragma once
#include <random>
#include <Common.hpp>

const Move &get_move(int move_nb);
const Move &get_move_restricted(int move_nb);
const Move &get_skewb_move(int move_nb);

Face opposite_face(Face face);
bool should_search_with_move(const Move& next_mone, const Move& prev_move);

template <typename GetMoveFn>
std::vector<Move> generate_shuffle(GetMoveFn get_move_fn, std::size_t moveset_size, std::size_t shuffle_size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, moveset_size - 1);

	std::vector<Move> Moves;
	Moves.reserve(shuffle_size);

	while (Moves.size() != shuffle_size)
	{
		Move move = get_move_fn(dist(gen));

		if (Moves.size() >= 1 && Moves.back().face == move.face)
			continue;

		Moves.push_back(move);
	}

	return Moves;
}
