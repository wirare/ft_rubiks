#include <Rubiks.hpp>
#include <cstddef>
#include <vector>
#include <optional>

Move &get_move(int move_nb)
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
	}

	return move_list[move_nb];
}

std::optional<std::vector<Move>> IDDFS_Search(const Rubiks &initial_state, std::size_t depth)
{
	std::vector<Move> Moves;
	Rubiks current_state = initial_state;


}
