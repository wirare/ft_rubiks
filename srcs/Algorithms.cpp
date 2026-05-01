#include <Rubiks.hpp>
#include <algorithm>
#include <climits>
#include <cstddef>
#include <utility>
#include <vector>
#include <Utility.hpp>
#include <Ranks.hpp>
#include <Tables.hpp>
#include <Algorithms.hpp>

inline Face opposite_face(Face face)
{
	if (face % 2 == 0)
		return (Face)((int)face+1);
	return (Face)((int)face-1);
}

inline bool should_search_with_move(const Move& next_move, const Move& prev_move)
{
	if (next_move.face == prev_move.face)
		return false;

	if (next_move.face == opposite_face(prev_move.face) && (int)prev_move.face > (int)next_move.face)
		return false;
	return true;
}

std::pair<bool, std::vector<Move>> IDDFS_Search(const Rubiks &initial_state, std::size_t current_depth, const std::size_t max_depth, std::vector<Move> &Moves)
{
	Rubiks current_state = initial_state;

	for (int i = 0; i != 18; i++)
	{
		const Move &next_move = get_move(i);
		if (Moves.size() >= 1 && !should_search_with_move(next_move, Moves.back()))
			continue;

		current_state.apply_move(next_move);
		Moves.push_back(next_move);
		if (current_state.is_solved())
			return std::make_pair(true, Moves);

		if (current_depth < max_depth)
		{
			auto result = IDDFS_Search(current_state, current_depth+1, max_depth, Moves);
			if (result.first == true)
				return result;
		}

		Moves.pop_back();
		current_state = initial_state;
	}
	return std::make_pair(false, Moves);
}

std::pair<bool, std::vector<Move>> Start_IDDFS(const Rubiks &initial_state)
{
	std::vector<Move> Moves;

	for (int i = 0; i != 20; i++)
	{
		auto res = IDDFS_Search(initial_state, 0, i, Moves);
		if (res.first == true)
			return res;
	}
	return std::make_pair(false, Moves);
}

static inline int phase1_heuristic(int corner_orientation, int edge_orientation, int slice)
{
	static const auto [twist_slice_prune, flip_slice_prune] = Tables::load_pruning_table_phase1();

	int h1 = twist_slice_prune[corner_orientation * 495 + slice];
	int h2 = flip_slice_prune[edge_orientation * 495 + slice];

	return std::max(h1, h2);
}

#define MAX_PHASE1_DEPTH 20

std::pair<bool, int> phase1_IDA_Star_Search(int corner_orientation, int edge_orientation, int slice, int depth, int bound, std::vector<Move> &Moves)
{
	static const auto [phase1_slice_move_table, orientation_tables] = Tables::load_move_table_phase1();
	static const auto [corner_orientation_move_table, edge_orientation_move_table] = orientation_tables;

	int h = phase1_heuristic(corner_orientation, edge_orientation, slice);

	if (depth + h > bound)
		return std::make_pair(false, depth + h);

	if (corner_orientation == 0 && edge_orientation == 0 && slice == 494)
		return std::make_pair(true, 0);

	int min_next_bound = INT_MAX;

	for (int i = 0; i != 18; i++)
	{
		Move move = get_move(i);
		if (Moves.size() >= 1 && !should_search_with_move(move, Moves.back()))
			continue;

		int next_corner_orientation = corner_orientation_move_table[corner_orientation][i];
		int next_edge_orientation = edge_orientation_move_table[edge_orientation][i];
		int next_slice = phase1_slice_move_table[slice][i];

		Moves.push_back(move);

		auto result = phase1_IDA_Star_Search(
						next_corner_orientation,
						next_edge_orientation,
						next_slice,
						depth + 1,
						bound,
						Moves
						);

		if (result.first == true)
			return result;

		min_next_bound = std::min(min_next_bound, result.second);

		Moves.pop_back();
	}

	return std::make_pair(false, min_next_bound);
}

std::pair<bool, std::vector<Move>> Start_phase1_IDA_Star(const Rubiks &initial_state)
{
	std::vector<Move> Moves;

	int corner_orientation	= Ranks::compute_corner_orientation_coordinate(initial_state);
	int edge_orientation	= Ranks::compute_edge_orientation_coordinate(initial_state);
	int slice				= Ranks::compute_UD_Slice_phase1_coordinate(initial_state);

	int bound = phase1_heuristic(corner_orientation, edge_orientation, slice);

	while (bound <= MAX_PHASE1_DEPTH)
	{
		auto result = phase1_IDA_Star_Search(corner_orientation, edge_orientation, slice, 0, bound, Moves);

		if (result.first == true)
			return std::make_pair(true, Moves);

		bound = result.second;
	}

	return std::make_pair(false, Moves);
}

static inline int phase2_heuristic(int corner_permutation, int UD_edge_permutation, int slice_permutation)
{
	static const auto [corner_slice_prune, edge_slice_prune] = Tables::load_pruning_table_phase2();

	int h1 = corner_slice_prune[corner_permutation * 24 + slice_permutation];
	int h2 = edge_slice_prune[UD_edge_permutation * 24 + slice_permutation];

	return std::max(h1, h2);
}

std::pair<bool, int> phase2_IDA_Star_Search(int corner_permutation, int UD_edge_permutation, int slice_permutation, int depth, int bound, std::vector<Move> &Moves)
{
	static const auto [phase2_slice_move_table, permutations_tables] = Tables::load_move_table_phase2();
	static const auto [corner_permutation_move_table, UD_edge_permutation_move_table] = permutations_tables;

	int h = phase2_heuristic(corner_permutation, UD_edge_permutation, slice_permutation);

	if (depth + h > bound)
		return std::make_pair(false, depth + h);

	if (corner_permutation == 0 && UD_edge_permutation == 0 && slice_permutation == 0)
		return std::make_pair(true, 0);

	int min_next_bound = INT_MAX;

	for (int i = 0; i != 10; i++)
	{
		Move move = get_move_restricted(i);
		if (Moves.size() >= 1 && !should_search_with_move(move, Moves.back()))
			continue;

		int next_corner_permutation = corner_permutation_move_table[corner_permutation][i];
		int next_UD_edge_permutation = UD_edge_permutation_move_table[UD_edge_permutation][i];
		int next_slice_permutation = phase2_slice_move_table[slice_permutation][i];

		Moves.push_back(move);

		auto result = phase2_IDA_Star_Search(
						next_corner_permutation,
						next_UD_edge_permutation,
						next_slice_permutation,
						depth + 1,
						bound,
						Moves
						);

		if (result.first == true)
			return result;

		min_next_bound = std::min(min_next_bound, result.second);

		Moves.pop_back();
	}

	return std::make_pair(false, min_next_bound);
}
#define MAX_PHASE2_DEPTH 20
std::pair<bool, std::vector<Move>> Start_phase2_IDA_Star(const Rubiks &phase1_cube)
{
	std::vector<Move> Moves;

	int corner_permutation	= Ranks::compute_corner_perm_coordinate(phase1_cube);
	int UD_edge_permutation	= Ranks::compute_UD_edge_perm_coordinate(phase1_cube);
	int slice_permutation	= Ranks::compute_UD_Slice_phase2_coordinate(phase1_cube);

	int bound = phase2_heuristic(corner_permutation, UD_edge_permutation, slice_permutation);

	while (bound <= MAX_PHASE2_DEPTH)
	{
		auto result = phase2_IDA_Star_Search(corner_permutation, UD_edge_permutation, slice_permutation, 0, bound, Moves);

		if (result.first == true)
			return std::make_pair(true, Moves);

		bound = result.second;
	}

	return std::make_pair(false, Moves);
}
