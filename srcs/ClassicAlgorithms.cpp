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
	int h1 = ClassicTables::twist_slice_pruning_table()[corner_orientation * 495 + slice];
	int h2 = ClassicTables::flip_slice_pruning_table()[edge_orientation * 495 + slice];

	return std::max(h1, h2);
}

#define MAX_PHASE1_DEPTH 12

std::pair<bool, int> phase1_IDA_Star_Search(int corner_orientation, int edge_orientation, int slice, int depth, int bound, std::vector<Move> &Moves, std::vector<std::vector<Move>> &solution_list, bool collect_solutions)
{
	int h = phase1_heuristic(corner_orientation, edge_orientation, slice);

	if (depth + h > bound)
		return std::make_pair(false, depth + h);

	if (corner_orientation == 0 && edge_orientation == 0 && slice == 494)
	{
		if (collect_solutions)
		{
			solution_list.push_back(Moves);
			return std::make_pair(false, INT_MAX);
		}

		return std::make_pair(true, 0);
	}

	int min_next_bound = INT_MAX;

	for (int i = 0; i != 18; i++)
	{
		Move move = get_move(i);
		if (Moves.size() >= 1 && !should_search_with_move(move, Moves.back()))
			continue;

		int next_corner_orientation = ClassicTables::corner_orientation_move_table()[corner_orientation][i];
		int next_edge_orientation = ClassicTables::edge_orientation_move_table()[edge_orientation][i];
		int next_slice = ClassicTables::phase1_slice_move_table()[slice][i];

		Moves.push_back(move);

		auto result = phase1_IDA_Star_Search(
						next_corner_orientation,
						next_edge_orientation,
						next_slice,
						depth + 1,
						bound,
						Moves,
						solution_list,
						collect_solutions
						);

		if (!collect_solutions && result.first == true)
			return result;

		min_next_bound = std::min(min_next_bound, result.second);

		Moves.pop_back();
	}

	return std::make_pair(false, min_next_bound);
}

std::pair<bool, std::vector<Move>> Start_phase1_IDA_Star(const Rubiks &initial_state)
{
	std::vector<Move> Moves;
	std::vector<std::vector<Move>> unused;

	int corner_orientation	= ClassicRanks::compute_corner_orientation_coordinate(initial_state);
	int edge_orientation	= ClassicRanks::compute_edge_orientation_coordinate(initial_state);
	int slice				= ClassicRanks::compute_UD_Slice_phase1_coordinate(initial_state);

	int bound = phase1_heuristic(corner_orientation, edge_orientation, slice);

	while (bound <= MAX_PHASE1_DEPTH)
	{
		auto result = phase1_IDA_Star_Search(corner_orientation, edge_orientation, slice, 0, bound, Moves, unused, false);

		if (result.first == true)
			return std::make_pair(true, Moves);

		bound = result.second;
	}

	return std::make_pair(false, Moves);
}

std::pair<bool, std::vector<std::vector<Move>>> Collect_phase1_IDA_Star(const Rubiks &initial_state, int max_depth = MAX_PHASE1_DEPTH)
{
	std::vector<Move> Moves;
	std::vector<std::vector<Move>> solution_list;

	int corner_orientation	= ClassicRanks::compute_corner_orientation_coordinate(initial_state);
	int edge_orientation	= ClassicRanks::compute_edge_orientation_coordinate(initial_state);
	int slice				= ClassicRanks::compute_UD_Slice_phase1_coordinate(initial_state);

	phase1_IDA_Star_Search(corner_orientation, edge_orientation, slice, 0, max_depth, Moves, solution_list, true);

	return std::make_pair(!solution_list.empty(), solution_list);
}

static inline int phase2_heuristic(int corner_permutation, int UD_edge_permutation, int slice_permutation)
{
	int h1 = ClassicTables::corner_slice_pruning_table()[corner_permutation * 24 + slice_permutation];
	int h2 = ClassicTables::UD_edge_slice_pruning_table()[UD_edge_permutation * 24 + slice_permutation];

	return std::max(h1, h2);
}

std::pair<bool, int> phase2_IDA_Star_Search(int corner_permutation, int UD_edge_permutation, int slice_permutation, int depth, int bound, std::vector<Move> &Moves)
{
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

		int next_corner_permutation = ClassicTables::corner_permutation_move_table()[corner_permutation][i];
		int next_UD_edge_permutation = ClassicTables::UD_edge_permutation_move_table()[UD_edge_permutation][i];
		int next_slice_permutation = ClassicTables::phase2_slice_move_table()[slice_permutation][i];

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

std::pair<bool, std::vector<Move>> Start_phase2_IDA_Star(const Rubiks &phase1_cube, int max_phase2_depth)
{
	std::vector<Move> Moves;

	int corner_permutation	= ClassicRanks::compute_corner_perm_coordinate(phase1_cube);
	int UD_edge_permutation	= ClassicRanks::compute_UD_edge_perm_coordinate(phase1_cube);
	int slice_permutation	= ClassicRanks::compute_UD_Slice_phase2_coordinate(phase1_cube);

	int bound = phase2_heuristic(corner_permutation, UD_edge_permutation, slice_permutation);

	while (bound <= max_phase2_depth)
	{
		auto result = phase2_IDA_Star_Search(corner_permutation, UD_edge_permutation, slice_permutation, 0, bound, Moves);

		if (result.first == true)
			return std::make_pair(true, Moves);

		bound = result.second;
	}

	return std::make_pair(false, Moves);
}

std::pair<bool, std::vector<Move>> Find_better_solution(const Rubiks &initial_state)
{
	std::vector<Move> Moves;
	auto phase1_candidates = Collect_phase1_IDA_Star(initial_state);
	std::size_t best_length = INT_MAX;
	std::vector<Move> best_phase1;
	std::vector<Move> best_phase2;
	
	if (!phase1_candidates.first)
		return std::make_pair(false, Moves);

	for (const auto &phase1_path : phase1_candidates.second)
	{
		Rubiks phase1_cube = initial_state;
		phase1_cube.apply_move_vector(phase1_path);

		int max_phase2_depth = best_length - phase1_path.size() - 1;

		if (max_phase2_depth < 0)
			continue;

		std::pair<bool, std::vector<Move>> phase2_result = Start_phase2_IDA_Star(phase1_cube, max_phase2_depth);

		if (phase2_result.first)
		{
			std::size_t total = phase1_path.size() + phase2_result.second.size();

			if (total < best_length)
			{
				best_length = total;
				best_phase1 = phase1_path;
				best_phase2 = phase2_result.second;
			}
		}
	}

	if (best_phase2.size() == 0)
		return std::make_pair(false, Moves);

	Moves = best_phase1;
	Moves.insert(Moves.end(), best_phase2.begin(), best_phase2.end());
	return std::make_pair(true, Moves);
}
