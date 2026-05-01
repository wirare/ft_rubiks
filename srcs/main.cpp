#include <Rubiks.hpp>
#include <Algorithms.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <Tables.hpp>

#define CASE_FACE(x) case *#x: tmp.face = x; break
std::vector<Move> parse_moves(const std::string& moves_str)
{
	std::vector<Move> moves;

	std::string token;
	std::stringstream ss(moves_str);
	Move tmp;

	while (std::getline(ss, token, ' '))
	{
		if (token.length() > 2)
			throw std::invalid_argument("Error in the shuffle string: Move too big");

		switch (token[0])
		{
			CASE_FACE(U);
			CASE_FACE(D);
			CASE_FACE(L);
			CASE_FACE(R);
			CASE_FACE(F);
			CASE_FACE(B);
			default:
				throw std::invalid_argument("Error in the shuffle string: Wrong face");
		}

		if (token.length() == 2)
		{
			switch (token[1])
			{
				case '\'': tmp.modifier = COUNTER; break;
				case '2': tmp.modifier = TWICE; break;
				default:
					throw std::invalid_argument("Error in the shuffle string: Wrong modifier");
			}
		}
		else
			tmp.modifier = NONE;

		moves.push_back(tmp);
	}

	return moves;
}
#undef CASE_FACE

#define COMPUTE_WRITE_TABLE(x)			\
{										\
	auto table = Tables::build_##x();	\
	Tables::write_table("Tables/"#x".bin", table);		\
}

int main(int ac, char **av)
{
	if (ac < 2)
		return 1;
	
	static const auto [corner_slice_prune, edge_slice_prune] = Tables::load_pruning_table_phase2();
	if (std::find(corner_slice_prune.begin(), corner_slice_prune.end(), 255) != corner_slice_prune.end())
	{
		std::cout << "Error in corner slice prune table\n";
		return 1;
	}
	if (std::find(edge_slice_prune.begin(), edge_slice_prune.end(), 255) != edge_slice_prune.end())
	{
		std::cout << "Error in edge slice prune table\n";
		return 1;
	}

	std::vector<Move> moves = parse_moves(std::string(av[1]));
	Rubiks cube;
	cube.apply_move_vector(moves);

	auto phase1_result = Start_phase1_IDA_Star(cube);
	if (phase1_result.first == false)
	{
		std::cout << "Couldnt solve phase1\n";
		return 1;
	}

	cube.apply_move_vector(phase1_result.second);
	auto phase2_result = Start_phase2_IDA_Star(cube);
	if (phase2_result.first == false)
	{
		std::cout << "Couldnt solve phase2\n";
		return 2;
	}

	cube.apply_move_vector(phase2_result.second);
	if (cube.is_solved())
	{
		std::cout << "Cube solved in " << phase1_result.second.size() + phase2_result.second.size() << " moves\n";
		for (Move &move : phase1_result.second)
			std::cout << static_cast<std::string>(move) << " ";
		for (Move &move : phase2_result.second)
			std::cout << static_cast<std::string>(move) << " ";
		std::cout << std::endl;
	}
	/*
	(void)ac;
	(void)av;
	COMPUTE_WRITE_TABLE(corners_orientations_move_table);
	COMPUTE_WRITE_TABLE(edges_orientations_move_table);
	COMPUTE_WRITE_TABLE(phase1_slice_move_table);
	COMPUTE_WRITE_TABLE(twist_slice_pruning_table);
	COMPUTE_WRITE_TABLE(flip_slice_pruning_table);
	*/
}
