#include "Common.hpp"
#include "Skewb.hpp"
#include <Rubiks.hpp>
#include <Algorithms.hpp>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <Tables.hpp>
#include <Utility.hpp>
#include <Puzzle.hpp>

#define COMPUTE_WRITE_TABLE(t, x)									\
{																	\
	auto table = t##Tables::build_##x();							\
	t##Tables::write_vector_table(#t"Tables/"#x".bin", table);	\
}

int load_write_tables(CubeType type)
{
	try
	{
		switch (type)
		{
			case CLASSIC:
				ClassicTables::load_all_tables(); break;
			case SKEWB:
				SkewbTables::load_all_tables(); break;
		}
		return 0;
	}
	catch (const std::exception& load_e)
	{
		std::cerr << "Could not load tables: " << load_e.what() << std::endl;
		std::cerr << "Generating tables..." << std::endl;

		try
		{
			switch (type)
			{
				case CLASSIC:
					{
						std::filesystem::create_directories("ClassicTables");
						COMPUTE_WRITE_TABLE(Classic, corner_orientation_move_table);
						COMPUTE_WRITE_TABLE(Classic, edge_orientation_move_table);
						COMPUTE_WRITE_TABLE(Classic, phase1_slice_move_table);
						COMPUTE_WRITE_TABLE(Classic, corner_permutation_move_table);
						COMPUTE_WRITE_TABLE(Classic, UD_edge_permutation_move_table);
						COMPUTE_WRITE_TABLE(Classic, phase2_slice_move_table);

						COMPUTE_WRITE_TABLE(Classic, twist_slice_pruning_table);
						COMPUTE_WRITE_TABLE(Classic, flip_slice_pruning_table);
						COMPUTE_WRITE_TABLE(Classic, corner_slice_pruning_table);
						COMPUTE_WRITE_TABLE(Classic, UD_edge_slice_pruning_table);
						break;
					}
				case SKEWB:
					{
						std::filesystem::create_directories("SkewbTables");
						COMPUTE_WRITE_TABLE(Skewb, corner_orientation_move_table);
						COMPUTE_WRITE_TABLE(Skewb, corner_permutation_move_table);
						COMPUTE_WRITE_TABLE(Skewb, center_permutation_move_table);

						COMPUTE_WRITE_TABLE(Skewb, orientation_center_pruning_table);
						COMPUTE_WRITE_TABLE(Skewb, orientation_permutation_pruning_table);
						break;
					}
			}

			SkewbTables::load_all_tables();
			return 0;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error while generating/loading tables: "
				<< e.what() << std::endl;
			return 1;
		}
	}
}

void print_help()
{
	std::cout	<< "Usage: ./Rubiks [ Classic | Skewb ] [ --random-shuffle [n] | -rs [n] | {A valid shuffle} ]\n"
				<< "A shuffle if considered valid for : \n"
				<< "Classic: Use this move set [ U | D | L | R | F | B ] with a possible combination of those modifier [ ' | 2 ]\n"
				<< "Skewb: Use this move set [ U | L | R | B ] with a possible combination of this modifier [ ' ]\n"
				<< "For random shuffle, if not specified, the shuffle will use a lenght of 20, you can also specify a size n (n > 0 && n < INT_MAX)\n";
}

int main(int ac, char **av)
{
	if (ac < 3)
	{
		print_help();
		return 1;
	}

	Puzzle *puzzle;

	if (std::string(av[1]) == "Classic")
		puzzle = new Rubiks;
	else if (std::string(av[1]) == "Skewb")
		puzzle = new Skewb;
	else
	{
		print_help();
		return 1;
	}

	std::vector<Move> shuffle;
	if (std::string(av[2]) == "--random-shuffle" || std::string(av[2]) == "-rs")
	{
		int n = 20;

		if (ac >= 4)
			n = std::atoi(av[3]);

		if (n <= 0)
		{
			print_help();
			delete puzzle;
			return 1;
		}

		shuffle = generate_shuffle([puzzle](int i) -> const Move& { return puzzle->move_generator(i); }, puzzle->get_moveset_size(), n);
	}
	else
	{
		try
		{
			shuffle = puzzle->parse_moves(std::string(av[2]));
		}
		catch (const std::exception& e)
		{
			std::cout << "Error during shuffle parsing: " << e.what() << std::endl;
			print_help();
			delete puzzle;
			return 1;
		};
	}


	try 
	{
		puzzle->print_move_vector(shuffle);
		puzzle->apply_move_vector(shuffle);
		load_write_tables(puzzle->get_type());
		puzzle->solve();
	}
	catch (const std::exception& e)
	{
		std::cout << "Error during solution search: " << e.what() << std::endl;
	}

	delete puzzle;
	return 0;
}
