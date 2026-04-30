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
		std::array<std::array<int, 18>, 2187> corner_orientation_move_table;

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
		std::array<std::array<int, 18>, 2048> edge_orientation_move_table;

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
		std::array<std::array<int, 18>, 495> phase1_slice_move_table;

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
		std::array<std::array<int, 10>, 40320> corners_permutation_move_table;

		for (int i = 0; i != 40320; i++)
		{
			Rubiks cube;
			cube.set_8_corners(unrank_corners_perm_coordinate(i));

			for (int m = 0; m != 10; m++)
			{
				Move move = get_move_restricted(m);
				Rubiks moved_cube = cube;
				moved_cube.apply_move(move);
				corners_permutation_move_table[i][m] = compute_corner_perm_coordinate(cube);
			}
		}

		return corners_permutation_move_table;
	}

	static inline auto build_twist_slice_pruning_table()
	{
		auto corner_orientation_move_table = build_corners_orientations_move_table();
		auto phase1_slice_move_table = build_phase1_slice_move_table();

		std::array<uint8_t, 2187 * 495> twist_slice_pruning_table;
		std::queue<std::pair<int, int>> BFS_Queue;

		twist_slice_pruning_table.fill(255);

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

		std::array<uint8_t, 2048 * 495> flip_slice_pruning_table;
		std::queue<std::pair<int, int>> BFS_Queue;

		flip_slice_pruning_table.fill(255);

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

	static inline auto load_pruning_table_phase1()
	{
		return std::make_pair(build_twist_slice_pruning_table(), build_flip_slice_pruning_table());
	}

	static inline auto load_move_table_phase1()
	{
		return std::make_pair(build_phase1_slice_move_table(), std::make_pair(build_corners_orientations_move_table(), build_edges_orientations_move_table()));
	}
};
