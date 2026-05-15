#pragma once

#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <string>
#include <Common.hpp>

struct Settings
{
	Settings() = default;
	Settings(int ac, char **av)
	{
		if (ac < 2)
			throw std::invalid_argument("Not enough argument");

		for (int i = 1; i < ac; i++)
			args.push(std::string(av[i]));

		while(!args.empty())
		{
			std::string current = args.top();
			args.pop();

			if (current == "Skewb")
				type = SKEWB;

			else if (current == "Classic")
				type = CLASSIC;

			else if (current == "--random-shuffle" || current == "-rs")
			{
				is_random_shuffle = true;

				if (!args.empty())
				{
					int n = std::atoi(args.top().c_str());

					if (n > 0)
					{
						shuffle_size = n;
						args.pop();
					}
				}
			}

			else if (current == "-b")
				use_better_move_alg = true;

			else if (current == "--readable")
				human_instinctive_print = true;

			else if (current == "--lang-fr")
				language_g = LANG_FR;
			
			else if (current == "--lang-en")
				language_g = LANG_EN;

			else
				user_shuffle = current;
		}
	}

	CubeType	type = CLASSIC;
	bool		is_random_shuffle = true;
	std::size_t	shuffle_size = 20;
	std::string	user_shuffle = "";
	bool		use_better_move_alg = false;
	bool		human_instinctive_print = false;

	private:
		std::stack<std::string>	args;
};

