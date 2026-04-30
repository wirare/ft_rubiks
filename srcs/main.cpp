#include <Rubiks.hpp>
#include <IDDFS.hpp>
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
	/*
	if (ac < 3)
		return 1;
	std::vector<Move> moves = parse_moves(std::string(av[1]));
	Rubiks cube;
	cube.apply_move_vector(moves);
	std::size_t depth = std::atoi(av[2]);

	auto result = Start_IDDFS(cube);
	if (result.first == true)
	{
		for (Move &move : result.second)
			std::cout << static_cast<std::string>(move) << " ";
		std::cout << std::endl;
	}
	else
		std::cout << "No solution found for this cube in a depth of " << depth << std::endl;
	*/
	(void)ac;
	(void)av;
	COMPUTE_WRITE_TABLE(corners_orientations_move_table);
	COMPUTE_WRITE_TABLE(edges_orientations_move_table);
	COMPUTE_WRITE_TABLE(phase1_slice_move_table);
	COMPUTE_WRITE_TABLE(twist_slice_pruning_table);
	COMPUTE_WRITE_TABLE(flip_slice_pruning_table);
}
