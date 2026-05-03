#pragma once
#include <Rubiks.hpp>
#include <Utility.hpp>
#include <cstdint>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <Ranks.hpp>
#include <string>

using namespace CommonRanks;

namespace CommonTables
{
	template <typename K>
	static inline void write_vector_table(const std::string& path, const std::vector<K>& table)
	{
		std::ofstream file(path, std::ios::binary);
		if (!file)
			throw std::runtime_error("Can't open the file to write the table: " + path);

		file.write(
			reinterpret_cast<const char*>(table.data()),
			static_cast<std::streamsize>(sizeof(K) * table.size())
		);

		if (!file)
			throw std::runtime_error("Error while writing the table: " + path);
	}

	template <typename T>
	static inline std::vector<T> read_vector_table(const std::string& path, std::size_t size)
	{
		std::vector<T> table(size);

		std::ifstream table_file(path, std::ios::binary | std::ios::ate);
		if (!table_file)
			throw std::runtime_error("Can't open the file to read the table");

		std::streamsize file_size = table_file.tellg();
		std::streamsize expected_size = static_cast<std::streamsize>(
				sizeof(T) * table.size()
				);

		if (file_size != expected_size)
			throw std::runtime_error("Table file has incorrect size");

		table_file.seekg(0, std::ios::beg);

		table_file.read(
				reinterpret_cast<char*>(table.data()),
				expected_size
				);

		if (!table_file)
			throw std::runtime_error("Error while reading the table");

		return table;
	}



	template <
		int move_set_size,
		int table_size,
		typename CubeType,
		typename UnrankFn,
		typename ApplyFn,
		typename ComputeFn,
		typename MoveGetterFn
		>
	static inline auto build_generic_move_table(
			UnrankFn unrank,
			ApplyFn apply,
			ComputeFn compute,
			MoveGetterFn move_getter
	)
	{
		std::vector<std::array<int, move_set_size>> move_table(table_size);

		for (int i = 0; i != table_size; i++)
		{
			CubeType cube;
			(cube.*apply)(unrank(i));

			for (int m = 0; m != move_set_size; m++)
			{
				Move move = move_getter(m);
				CubeType moved_cube = cube;
				moved_cube.apply_move(move);
				move_table[i][m] = compute(moved_cube);
			}
		}

		return move_table;
	}

	template <
		typename BuildPieceMoveTableFn,
		typename BuildSliceMoveTableFn
	>
	static inline std::vector<uint8_t> build_generic_pruning_table(
			BuildPieceMoveTableFn build_piece_move_table_fn, 
			BuildSliceMoveTableFn build_slice_move_table_fn, 
			int piece_perm_nb, int slice_perm_nb, 
			int move_set_size,
			const std::pair<int, int> &solved_state
	)
	{
		auto piece_move_table = build_piece_move_table_fn();
		auto slice_move_table = build_slice_move_table_fn();

		std::vector<uint8_t> pruning_table(piece_perm_nb * slice_perm_nb, 255);
		std::queue<std::pair<int, int>> BFS_Queue;

		BFS_Queue.push(solved_state);
		pruning_table[solved_state.second] = 0;

		while (!BFS_Queue.empty())
		{
			auto [piece, slice] = BFS_Queue.front();
			BFS_Queue.pop();

			int current_index = piece * slice_perm_nb + slice;
			uint8_t current_distance = pruning_table[current_index];

			for (int move = 0; move != move_set_size; move++)
			{
				int next_piece = piece_move_table[piece][move];
				int next_slice = slice_move_table[slice][move];

				int next_index = next_piece * slice_perm_nb + next_slice;

				if (pruning_table[next_index] == 255)
				{
					pruning_table[next_index] = current_distance + 1;
					BFS_Queue.push({next_piece, next_slice});
				}
			}
		}

		return pruning_table;
	}
};

namespace ClassicTables
{
	using namespace CommonTables;
	using namespace ClassicRanks;
	// -------------------------------------------------------------------------
	// Move tables
	// -------------------------------------------------------------------------

	static inline const std::vector<std::array<int, 18>>& corner_orientation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 18>>(
				"ClassicTables/corner_orientation_move_table.bin",
				2187
			);

		return table;
	}

	static inline const std::vector<std::array<int, 18>>& edge_orientation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 18>>(
				"ClassicTables/edge_orientation_move_table.bin",
				2048
			);

		return table;
	}

	static inline const std::vector<std::array<int, 18>>& phase1_slice_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 18>>(
				"ClassicTables/phase1_slice_move_table.bin",
				495
			);

		return table;
	}

	static inline const std::vector<std::array<int, 10>>& corner_permutation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 10>>(
				"ClassicTables/corner_permutation_move_table.bin",
				40320
			);

		return table;
	}

	static inline const std::vector<std::array<int, 10>>& UD_edge_permutation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 10>>(
				"ClassicTables/UD_edge_permutation_move_table.bin",
				40320
			);

		return table;
	}

	static inline const std::vector<std::array<int, 10>>& phase2_slice_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 10>>(
				"ClassicTables/phase2_slice_move_table.bin",
				24
			);

		return table;
	}

	// -------------------------------------------------------------------------
	// Pruning tables
	// -------------------------------------------------------------------------

	static inline const std::vector<uint8_t>& twist_slice_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
				"ClassicTables/twist_slice_pruning_table.bin",
				2187 * 495
			);

		return table;
	}

	static inline const std::vector<uint8_t>& flip_slice_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
				"ClassicTables/flip_slice_pruning_table.bin",
				2048 * 495
			);

		return table;
	}

	static inline const std::vector<uint8_t>& corner_slice_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
				"ClassicTables/corner_slice_pruning_table.bin",
				40320 * 24
			);

		return table;
	}

	static inline const std::vector<uint8_t>& UD_edge_slice_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
				"ClassicTables/UD_edge_slice_pruning_table.bin",
				40320 * 24
			);

		return table;
	}

	// -------------------------------------------------------------------------
	// Explicit loading
	// -------------------------------------------------------------------------

	static inline void load_all_tables()
	{
		(void)corner_orientation_move_table();
		(void)edge_orientation_move_table();
		(void)phase1_slice_move_table();

		(void)corner_permutation_move_table();
		(void)UD_edge_permutation_move_table();
		(void)phase2_slice_move_table();

		(void)twist_slice_pruning_table();
		(void)flip_slice_pruning_table();

		(void)corner_slice_pruning_table();
		(void)UD_edge_slice_pruning_table();
	}

	static inline auto build_corner_orientation_move_table()
	{
		return build_generic_move_table<18, 2187, Rubiks>(unrank_corners_orientation, &Rubiks::apply_corner_orientation, compute_corner_orientation_coordinate, get_move);
	}

	static inline auto build_edge_orientation_move_table()
	{
		return build_generic_move_table<18, 2048, Rubiks>(unrank_edges_orientation, &Rubiks::apply_edge_orientation, compute_edge_orientation_coordinate, get_move);
	}

	static inline auto build_phase1_slice_move_table()
	{
		return build_generic_move_table<18, 495, Rubiks>(unrank_UD_Slice_phase1, &Rubiks::set_4_edges, compute_UD_Slice_phase1_coordinate, get_move);
	}

	static inline auto build_corner_permutation_move_table()
	{
		return build_generic_move_table<10, 40320, Rubiks>(unrank_corners_perm_coordinate, &Rubiks::set_8_corners, compute_corner_perm_coordinate, get_move_restricted);
	}

	static inline auto build_UD_edge_permutation_move_table()
	{
		return build_generic_move_table<10, 40320, Rubiks>(unrank_UD_edge_perm_coordinate, &Rubiks::set_8_edges, compute_UD_edge_perm_coordinate, get_move_restricted);
	}

	static inline auto build_phase2_slice_move_table()
	{
		return build_generic_move_table<10, 24, Rubiks>(unrank_UD_Slice_phase2, &Rubiks::set_4_edges_ordered, compute_UD_Slice_phase2_coordinate, get_move_restricted);
	}

	static inline std::vector<uint8_t> build_twist_slice_pruning_table()
	{
		return	build_generic_pruning_table(build_corner_orientation_move_table,
											build_phase1_slice_move_table,
											2187, 495, 18,
											{0, 494});
	}

	static inline std::vector<uint8_t> build_flip_slice_pruning_table()
	{
		return	build_generic_pruning_table(build_edge_orientation_move_table,
											build_phase1_slice_move_table,
											2048, 495, 18,
											{0, 494});
	}

	static inline std::vector<uint8_t> build_corner_slice_pruning_table()
	{
		return	build_generic_pruning_table(build_corner_permutation_move_table,
											build_phase2_slice_move_table,
											40320, 24, 10,
											{0, 0});
	}

	static inline std::vector<uint8_t> build_UD_edge_slice_pruning_table()
	{
		return	build_generic_pruning_table(build_UD_edge_permutation_move_table,
											build_phase2_slice_move_table,
											40320, 24, 10,
											{0, 0});
	}
}

namespace SkewbTables
{
	using namespace CommonTables;
	using namespace SkewbRanks;

	// -------------------------------------------------------------------------
	// Move tables
	// -------------------------------------------------------------------------

	static inline const std::vector<std::array<int, 8>>& corner_orientation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 8>>(
					"SkewbTables/corner_orientation_move_table.bin",
					2187
					);

		return table;
	}

	static inline const std::vector<std::array<int, 8>>& center_permutation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 8>>(
					"SkewbTables/center_permutation_move_table.bin",
					720
					);

		return table;
	}

	static inline const std::vector<std::array<int, 8>>& corner_permutation_move_table()
	{
		static const auto table =
			read_vector_table<std::array<int, 8>>(
					"SkewbTables/corner_permutation_move_table.bin",
					36
					);

		return table;
	}

	// -------------------------------------------------------------------------
	// Pruning tables
	// -------------------------------------------------------------------------

	static inline const std::vector<uint8_t>& orientation_center_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
					"SkewbTables/orientation_center_pruning_table.bin",
					2187 * 720
					);

		return table;
	}

	static inline const std::vector<uint8_t>& orientation_permutation_pruning_table()
	{
		static const auto table =
			read_vector_table<uint8_t>(
					"SkewbTables/orientation_permutation_pruning_table.bin",
					2187 * 36
					);

		return table;
	}

	// -------------------------------------------------------------------------
	// Explicit loading
	// -------------------------------------------------------------------------

	static inline void load_all_tables()
	{
		(void)corner_orientation_move_table();
		(void)center_permutation_move_table();
		(void)corner_permutation_move_table();

		(void)orientation_center_pruning_table();
		(void)orientation_permutation_pruning_table();
	}
	static inline auto build_corner_orientation_move_table()
	{
		return build_generic_move_table<8, 2187, Skewb>(unrank_corners_orientation, &Skewb::set_corner_orientation, compute_corner_orientation_coordinate, get_skewb_move);
	}

	static inline auto build_center_permutation_move_table()
	{
		return build_generic_move_table<8, 720, Skewb>(unrank_centers_perm_coordinate, &Skewb::set_centers, compute_center_perm_coordinate, get_skewb_move);
	}

	static inline auto build_corner_permutation_move_table()
	{
		return build_generic_move_table<8, 36, Skewb>(unrank_corners_perm_coordinate, &Skewb::set_corner_perm, compute_corner_perm_coordinate, get_skewb_move);
	}

	static inline std::vector<uint8_t> build_orientation_center_pruning_table()
	{
		return	build_generic_pruning_table(build_corner_orientation_move_table,
											build_center_permutation_move_table,
											2187, 720, 8,
											{0, 0});
	}

	static inline std::vector<uint8_t> build_orientation_permutation_pruning_table()
	{
		return	build_generic_pruning_table(build_corner_orientation_move_table,
											build_corner_permutation_move_table,
											2187, 36, 8,
											{0, 0});
	}
}
