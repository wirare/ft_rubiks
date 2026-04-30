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
		(Move){U, NONE}, {U, COUNTER}, {U, TWICE},
		{D, NONE}, {D, COUNTER}, {D, TWICE},
		{F, TWICE},
		{B, TWICE},
		{R, TWICE},
		{L, TWICE}
	};

	return move_list[move_nb];
}
