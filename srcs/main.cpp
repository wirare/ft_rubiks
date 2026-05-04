#include "Skewb.hpp"
#include <Rubiks.hpp>
#include <Algorithms.hpp>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <Tables.hpp>
#include <Utility.hpp>

/*
#define COMPUTE_WRITE_TABLE(x)								\
{															\
	auto table = ClassicTables::build_##x();						\
	ClassicTables::write_vector_table("Tables/"#x".bin", table);	\
}

int main(int ac, char **av)
{
	if (ac < 2)
		return 1;

	try
	{
		ClassicTables::load_all_tables();
	}
	catch (const std::exception& load_e)
	{
		std::cerr << "Could not load tables: " << load_e.what() << std::endl;
		std::cerr << "Generating tables..." << std::endl;

		try
		{
			std::filesystem::create_directories("Tables");

			COMPUTE_WRITE_TABLE(corner_orientation_move_table);
			COMPUTE_WRITE_TABLE(edge_orientation_move_table);
			COMPUTE_WRITE_TABLE(phase1_slice_move_table);
			COMPUTE_WRITE_TABLE(corner_permutation_move_table);
			COMPUTE_WRITE_TABLE(UD_edge_permutation_move_table);
			COMPUTE_WRITE_TABLE(phase2_slice_move_table);

			COMPUTE_WRITE_TABLE(twist_slice_pruning_table);
			COMPUTE_WRITE_TABLE(flip_slice_pruning_table);
			COMPUTE_WRITE_TABLE(corner_slice_pruning_table);
			COMPUTE_WRITE_TABLE(UD_edge_slice_pruning_table);

			ClassicTables::load_all_tables();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error while generating/loading tables: "
				<< e.what() << std::endl;
			return 1;
		}
	}


	std::vector<Move> moves = parse_moves_classic(std::string(av[1]));
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
}
*/
#define COMPUTE_WRITE_TABLE(x)								\
{															\
	auto table = SkewbTables::build_##x();						\
	SkewbTables::write_vector_table("SkewbTables/"#x".bin", table);	\
}

int main(int ac, char **av)
{
	if (ac < 2)
		return 1;

	try
	{
		SkewbTables::load_all_tables();
	}
	catch (const std::exception& load_e)
	{
		std::cerr << "Could not load tables: " << load_e.what() << std::endl;
		std::cerr << "Generating tables..." << std::endl;

		try
		{
			std::filesystem::create_directories("SkewbTables");

			COMPUTE_WRITE_TABLE(corner_orientation_move_table);
			COMPUTE_WRITE_TABLE(corner_permutation_move_table);
			COMPUTE_WRITE_TABLE(center_permutation_move_table);

			COMPUTE_WRITE_TABLE(orientation_center_pruning_table);
			COMPUTE_WRITE_TABLE(orientation_permutation_pruning_table);

			SkewbTables::load_all_tables();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error while generating/loading tables: "
				<< e.what() << std::endl;
			return 1;
		}
	}

	std::vector<Move> moves = parse_moves_skewb(std::string(av[1]));
	Skewb cube;
	cube.apply_move_vector(moves);
	auto result = Start_skewb_IDA_Star(cube);
	if (result.first == false)
	{
		std::cout << "Couldnt solve skewb\n";
		return 1;
	}

	cube.apply_move_vector(result.second);
	if (!cube.is_solved())
	{
		std::cout << "Solver returned a path, but cube is not solved\n";
		return 1;
	}
	std::cout << "Cube solved in " << result.second.size() << " moves\n";
	for (Move &move : result.second)
		std::cout << print_skewb_move(move) << " ";
	std::cout << std::endl;
}
