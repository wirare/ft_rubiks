#include <Utility.hpp>
#include <Rubiks.hpp>

const Move &get_move(int move_nb)
{
	static std::array<Move, 18> move_list;
	static bool init = false;

	if (!init)
	{
		int counter = 0;

		for (int face = 0; face != 6; face++)
		{
			for (int modif = 1; modif != 4; modif++)
			{
				move_list[counter].face = (Face)face;
				move_list[counter].modifier = (Modifier)modif;
				counter++;
			}
		}
		init = true;
	}

	return move_list[move_nb];
}

const Move &get_move_restricted(int move_nb)
{
	static std::array<Move, 10> move_list = {
		(Move)
		{U, NONE}, {U, COUNTER}, {U, TWICE},
		{D, NONE}, {D, COUNTER}, {D, TWICE},
		{F, TWICE},
		{B, TWICE},
		{R, TWICE},
		{L, TWICE}
	};

	return move_list[move_nb];
}

const Move &get_skewb_move(int move_nb)
{
	static std::array<Move, 8> move_list {
		(Move)
		{U, NONE}, {U, TWICE},
		{B, NONE}, {B, TWICE},
		{R, NONE}, {R, TWICE},
		{L, NONE}, {L, TWICE},
	};

	return move_list[move_nb];
}

Face opposite_face(Face face)
{
	if (face % 2 == 0)
		return (Face)((int)face+1);
	return (Face)((int)face-1);
}

bool should_search_with_move(const Move& next_move, const Move& prev_move)
{
	if (next_move.face == prev_move.face)
		return false;

	if (next_move.face == opposite_face(prev_move.face) && (int)prev_move.face > (int)next_move.face)
		return false;
	return true;
}
