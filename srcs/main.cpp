#include <Rubiks.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define CASE_FACE(x) case *#x: tmp.face = x; break
std::vector<Move> parse_moves(const std::string& moves_str)
{
	std::vector<Move> moves;

	std::string token;
	std::stringstream ss(moves_str);
	Move tmp;

	while (std::getline(ss, token, ' '))
	{
		if (token.length() > 2)
			throw std::invalid_argument("Error in the shuffle string: Move too big");

		switch (token[0])
		{
			CASE_FACE(U);
			CASE_FACE(D);
			CASE_FACE(L);
			CASE_FACE(R);
			CASE_FACE(F);
			CASE_FACE(B);
			default:
				throw std::invalid_argument("Error in the shuffle string: Wrong face");
		}

		if (token.length() == 2)
		{
			switch (token[1])
			{
				case '\'': tmp.modifier = COUNTER; break;
				case '2': tmp.modifier = TWICE; break;
				default:
					throw std::invalid_argument("Error in the shuffle string: Wrong modifier");
			}
		}
		else
			tmp.modifier = NONE;

		moves.push_back(tmp);
	}

	return moves;
}
#undef CASE_FACE

int main(int ac, char **av)
{
	(void)ac;
	std::vector<Move> moves = parse_moves(std::string(av[1]));

	for (Move &move : moves)
	{
		std::cout << static_cast<std::string>(move) << " ";
	}
	std::cout << std::endl;
}
