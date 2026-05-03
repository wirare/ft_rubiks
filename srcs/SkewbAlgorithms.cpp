#include <Tables.hpp>
#include <Ranks.hpp>
#include <Skewb.hpp>
#include <Rubiks.hpp>
#include <Utility.hpp>
#include <Algorithms.hpp>
#include <algorithm>
#include <climits>

static inline int skewb_heuristic(int corner_orientation, int corner_permutation, int center_permutation)
{
	int h1 = SkewbTables::orientation_center_pruning_table()[corner_orientation * 720 + center_permutation];
	int h2 = SkewbTables::orientation_permutation_pruning_table()[corner_orientation * 36 + corner_permutation];

	return std::max(h1, h2);
}

std::pair<bool, int> skewb_IDA_Star_Search(int corner_orientation, int corner_permutation, int center_permutation, int depth, int bound, std::vector<Move> &Moves)
{
	int h = skewb_heuristic(corner_orientation, corner_permutation, center_permutation);

	if (depth + h > bound)
		return std::make_pair(false, depth + h);

	if (corner_orientation + corner_permutation + center_permutation == 0)
		return std::make_pair(true, 0);

	int min_next_bound = INT_MAX;

	for (int i = 0; i != 8; i++)
	{
		Move move = get_skewb_move(i);
		if (Moves.size() >= 1 && move.face == Moves.back().face)
			continue;

		int next_corner_orientation = SkewbTables::corner_orientation_move_table()[corner_orientation][i];
		int next_corner_permutation = SkewbTables::corner_permutation_move_table()[corner_permutation][i];
		int next_center_permutation = SkewbTables::center_permutation_move_table()[center_permutation][i];

		Moves.push_back(move);

		auto result = skewb_IDA_Star_Search(
						next_corner_orientation,
						next_corner_permutation,
						next_center_permutation,
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

#define MAX_SKEWB_DEPTH 11

std::pair<bool, std::vector<Move>> Start_skewb_IDA_Star(const Skewb &initial_state)
{
	std::vector<Move> Moves;

	int corner_orientation = SkewbRanks::compute_corner_orientation_coordinate(initial_state);
	int corner_permutation = SkewbRanks::compute_corner_perm_coordinate(initial_state);
	int center_permutation = SkewbRanks::compute_center_perm_coordinate(initial_state);

	int bound = skewb_heuristic(corner_orientation, corner_permutation, center_permutation);

	while (bound <= MAX_SKEWB_DEPTH)
	{
		auto result = skewb_IDA_Star_Search(corner_orientation, corner_permutation, center_permutation, 0, bound, Moves);

		if (result.first == true)
			return std::make_pair(true, Moves);

		bound = result.second;
	}

	return std::make_pair(false, Moves);
}
