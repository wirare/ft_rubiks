#pragma once
#include <cstddef>
#include <vector>
#include <string>
#include <Common.hpp>

struct Move;

class Puzzle
{
	public:
		virtual void apply_move_vector(const std::vector<Move> &moves) = 0;
		virtual void solve() = 0;
		virtual void solve_best() = 0;
		virtual const Move& move_generator(int i) const = 0;
		virtual std::size_t get_moveset_size() const = 0;
		virtual std::vector<Move> parse_moves(const std::string& moves_str) const = 0;
		virtual CubeType get_type() const = 0;
		virtual void print_move_vector(const std::vector<Move> &moves) const = 0;
		virtual ~Puzzle() {};
};
