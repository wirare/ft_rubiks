#pragma once
#include <Rubiks.hpp>
#include <Utility.hpp>
#include <cstdint>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <Ranks.hpp>

using namespace Ranks;

namespace Tables
{
	template <typename K>
	static inline void write_table(const std::string& path, const K &table)
	{
		std::ofstream file_to_store_table(path, std::ios::out | std::ios::binary);
		if (!file_to_store_table)
			throw std::runtime_error("Can't open the file to write the table");
		file_to_store_table.write(reinterpret_cast<const char *>(&table), sizeof(table));
		file_to_store_table.close();
	}

	static inline auto build_corners_orientations_move_table()
	{
		std::vector<std::array<int, 18>> corner_orientation_move_table(2187);

		for (int i = 0; i != 2187; i++)
		{
			Rubiks cube;
			cube.apply_corner_orientation(unrank_corners_orientation(i));

			for (int m = 0; m != 18; m++)
			{
				Move move = get_move(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				corner_orientation_move_table[i][m] = compute_corner_orientation_coordinate(moved_cube);
			}
		}

		return corner_orientation_move_table;
	}

	static inline auto build_edges_orientations_move_table()
	{
		std::vector<std::array<int, 18>> edge_orientation_move_table(2048);

		for (int i = 0; i != 2048; i++)
		{
			Rubiks cube;
			cube.apply_edge_orientation(unrank_edges_orientation(i));

			for (int m = 0; m != 18; m++)
			{
				Move move = get_move(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				edge_orientation_move_table[i][m] = compute_edge_orientation_coordinate(moved_cube);
			}
		}
		
		return edge_orientation_move_table;
	}

	static inline auto build_phase1_slice_move_table()
	{
		std::vector<std::array<int, 18>> phase1_slice_move_table(495);

		for (int i = 0; i != 495; i++)
		{
			Rubiks cube;
			cube.set_4_edges(unrank_UD_Slice_phase1(i));

			for (int m = 0; m != 18; m++)
			{
				Move move = get_move(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				phase1_slice_move_table[i][m] = compute_UD_Slice_phase1_coordinate(moved_cube);
			}
		}

		return phase1_slice_move_table;
	}

	static inline auto build_corners_permutation_move_table()
	{
		std::vector<std::array<int, 10>> corners_permutation_move_table(40320);

		for (int i = 0; i != 40320; i++)
		{
			Rubiks cube;
			cube.set_8_corners(unrank_corners_perm_coordinate(i));

			for (int m = 0; m != 10; m++)
			{
				Move move = get_move_restricted(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				corners_permutation_move_table[i][m] = compute_corner_perm_coordinate(moved_cube);
			}
		}

		return corners_permutation_move_table;
	}

	static inline auto build_UD_edges_permutation_move_table()
	{
		std::vector<std::array<int, 10>> UD_edges_permutation_move_table(40320);

		for (int i = 0; i != 40320; i++)
		{
			Rubiks cube;
			cube.set_8_edges(unrank_UD_edge_perm_coordinate(i));

			for (int m = 0; m != 10; m++)
			{
				Move move = get_move_restricted(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				UD_edges_permutation_move_table[i][m] = compute_UD_edge_perm_coordinate(moved_cube);
			}
		}

		return UD_edges_permutation_move_table;
	}

	static inline auto build_phase2_slice_move_table()
	{
		std::vector<std::array<int, 10>> phase2_slice_move_table(24);
		
		for (int i = 0; i != 24; i++)
		{
			Rubiks cube;
			cube.set_4_edges_ordered(unrank_UD_Slice_phase2(i));

			for (int m = 0; m != 10; m++)
			{
				Move move = get_move_restricted(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				phase2_slice_move_table[i][m] = compute_UD_Slice_phase2_coordinate(moved_cube);
			}
		}

		return phase2_slice_move_table;
	}

	static inline auto build_twist_slice_pruning_table()
	{
		auto corner_orientation_move_table = build_corners_orientations_move_table();
		auto phase1_slice_move_table = build_phase1_slice_move_table();

		std::vector<uint8_t> twist_slice_pruning_table(2187 * 495, 255);
		std::queue<std::pair<int, int>> BFS_Queue;

		BFS_Queue.push({0, 494});
		twist_slice_pruning_table[494] = 0;
		
		while (!BFS_Queue.empty())
		{
			auto [corner_orientation, slice] = BFS_Queue.front();
			BFS_Queue.pop();

			int current_index = corner_orientation * 495 + slice;
			uint8_t current_distance = twist_slice_pruning_table[current_index];

			for (int move = 0; move != 18; move++)
			{
				int next_corner_orientation = corner_orientation_move_table[corner_orientation][move];
				int next_slice = phase1_slice_move_table[slice][move];

				int next_index = next_corner_orientation * 495 + next_slice;

				if (twist_slice_pruning_table[next_index] == 255)
				{
					twist_slice_pruning_table[next_index] = current_distance + 1;
					BFS_Queue.push({next_corner_orientation, next_slice});
				}
			}
		}

		return twist_slice_pruning_table;
	}

	static inline auto build_flip_slice_pruning_table()
	{
		auto edge_orientation_move_table = build_edges_orientations_move_table();
		auto phase1_slice_move_table = build_phase1_slice_move_table();

		std::vector<uint8_t> flip_slice_pruning_table(2048 * 495, 255);
		std::queue<std::pair<int, int>> BFS_Queue;

		BFS_Queue.push({0, 494});
		flip_slice_pruning_table[494] = 0;
		
		while (!BFS_Queue.empty())
		{
			auto [edge_orientation, slice] = BFS_Queue.front();
			BFS_Queue.pop();

			int current_index = edge_orientation * 495 + slice;
			uint8_t current_distance = flip_slice_pruning_table[current_index];

			for (int move = 0; move != 18; move++)
			{
				int next_edge_orientation = edge_orientation_move_table[edge_orientation][move];
				int next_slice = phase1_slice_move_table[slice][move];

				int next_index = next_edge_orientation * 495 + next_slice;

				if (flip_slice_pruning_table[next_index] == 255)
				{
					flip_slice_pruning_table[next_index] = current_distance + 1;
					BFS_Queue.push({next_edge_orientation, next_slice});
				}
			}
		}

		return flip_slice_pruning_table;
	}

	static inline auto build_corner_slice_pruning_table()
	{
		auto corners_permutation_move_table = build_corners_permutation_move_table();
		auto phase2_slice_move_table = build_phase2_slice_move_table();

		std::vector<uint8_t> corner_slice_pruning_table(40320 * 24, 255);
		std::queue<std::pair<int, int>> BFS_Queue;

		BFS_Queue.push({0, 0});
		corner_slice_pruning_table[0] = 0;

		while (!BFS_Queue.empty())
		{
			auto [corner_permutation, slice_permutation] = BFS_Queue.front();
			BFS_Queue.pop();

			int current_index = corner_permutation * 24 + slice_permutation;
			uint8_t current_distance = corner_slice_pruning_table[current_index];

			for (int move = 0; move != 10; move++)
			{
				int next_corner_permutation = corners_permutation_move_table[corner_permutation][move];
				int next_slice_permutation = phase2_slice_move_table[slice_permutation][move];

				int next_index = next_corner_permutation * 24 + next_slice_permutation;

				if (corner_slice_pruning_table[next_index] == 255)
				{
					corner_slice_pruning_table[next_index] = current_distance + 1;
					BFS_Queue.push({next_corner_permutation, next_slice_permutation});
				}
			}
		}

		return corner_slice_pruning_table;
	}

	static inline auto build_edge_slice_pruning_table()
	{
		auto UD_edges_permutation_move_table = build_UD_edges_permutation_move_table();
		auto phase2_slice_move_table = build_phase2_slice_move_table();

		std::vector<uint8_t> edge_slice_pruning_table(40320 * 24, 255);
		std::queue<std::pair<int, int>> BFS_Queue;

		BFS_Queue.push({0, 0});
		edge_slice_pruning_table[0] = 0;

		while (!BFS_Queue.empty())
		{
			auto [edge_permutation, slice_permutation] = BFS_Queue.front();
			BFS_Queue.pop();

			int current_index = edge_permutation * 24 + slice_permutation;
			uint8_t current_distance = edge_slice_pruning_table[current_index];

			for (int move = 0; move != 10; move++)
			{
				int next_edge_permutation = UD_edges_permutation_move_table[edge_permutation][move];
				int next_slice_permutation = phase2_slice_move_table[slice_permutation][move];

				int next_index = next_edge_permutation * 24 + next_slice_permutation;

				if (edge_slice_pruning_table[next_index] == 255)
				{
					edge_slice_pruning_table[next_index] = current_distance + 1;
					BFS_Queue.push({next_edge_permutation, next_slice_permutation});
				}
			}
		}

		return edge_slice_pruning_table;
	}

	static inline auto load_pruning_table_phase1()
	{
		return std::make_pair(build_twist_slice_pruning_table(), build_flip_slice_pruning_table());
	}

	static inline auto load_move_table_phase1()
	{
		return std::make_pair(build_phase1_slice_move_table(), std::make_pair(build_corners_orientations_move_table(), build_edges_orientations_move_table()));
	}

	static inline auto load_pruning_table_phase2()
	{
		return std::make_pair(build_corner_slice_pruning_table(), build_edge_slice_pruning_table());
	}

	static inline auto load_move_table_phase2()
	{
		return std::make_pair(build_phase2_slice_move_table(), std::make_pair(build_corners_permutation_move_table(), build_UD_edges_permutation_move_table()));
	}
};
