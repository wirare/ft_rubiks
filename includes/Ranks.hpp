#pragma once

#include <Rubiks.hpp>

namespace Ranks
{
	const std::array<unsigned int, 13> factorial = 
	{
		1,
		1,
		2,
		6,
		24,
		120,
		720,
		5040,
		40320,
		362880,
		3628800,
		39916800,
		479001600
	};

	static int binomialCoef(int n, int k)
	{
		if (n < k || k < 0)
			return 0;
		return factorial[n] / (factorial[k] * factorial[n-k]);
	}

	template <typename Piece, int k>
	static std::vector<int> Lehmer_code(const std::array<Piece, k> &pieces_pos)
	{
		std::vector<int> code;
		code.reserve(k);

		for (int i = 0; i != k; i++)
		{
			int counter = 0;
			for (int j = i+1; j < k; j++)
				counter += (int)pieces_pos[i].pos > (int)pieces_pos[j].pos;
			code.push_back(counter);
		}

		return code;
	}

	static inline int compute_edge_orientation_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;
		for (int i = 0; i != 11; i++)
			coordinate += cube.get_edges()[i].orientation * std::pow(2, i);
		return coordinate;
	}

	static inline int compute_corner_orientation_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;
		for (int i = 0; i != 7; i++)
			coordinate += cube.get_corners()[i].orientation * std::pow(3, i);
		return coordinate;
	}

	static inline int compute_UD_Slice_phase1_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;

		auto idx = [cube](EdgePos p)
		{
			for (int i = 0; i != 12; i++)
			{
				if (cube.get_edges()[i].pos == p)
					return i;
			}
			__builtin_unreachable();
		};

		std::vector<int> indices;
		for (EdgePos p : {FR, FL, BR, BL})
			indices.push_back(idx(p));

		std::sort(indices.begin(), indices.end());

		for (int i = 0; i != 4; i++)
			coordinate += binomialCoef(indices[i], i+1);

		return coordinate;
	}

	static inline int compute_UD_Slice_phase2_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;

		std::array<Edge, 4> Non_UD_edges;
		int i = 0;
		for (EdgePos p : {FR, FL, BR, BL})
		{
			Non_UD_edges[i] = cube.get_edges()[p];
			i++;
		}

		std::vector<int> lehmer_code = Lehmer_code<Edge, 4>(Non_UD_edges);
		for (int i = 0; i != 4; i++)
			coordinate += lehmer_code[i] * factorial[3-i];

		return coordinate;
	}

	static inline int compute_corner_perm_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;

		std::vector<int> lehmer_code = Lehmer_code<Corner, 8>(cube.get_corners());
		for (int i = 0; i != 8; i++)
			coordinate += lehmer_code[i] * factorial[7 - i];

		return coordinate;
	}

	static inline int compute_UD_edge_perm_coordinate(const Rubiks& cube)
	{
		int coordinate = 0;

		std::array<Edge, 8> UD_edges;
		int i = 0;
		for (EdgePos p : {UR, UF, UL, UB, DR, DF, DL, DB})
		{
			UD_edges[i] = cube.get_edges()[p];
			i++;
		}

		std::vector<int> lehmer_code = Lehmer_code<Edge, 8>(UD_edges);
		for (int i = 0; i != 8; i++)
			coordinate += lehmer_code[i] * factorial[7 - i];

		return coordinate;
	}

	static inline std::array<int, 12> unrank_edges_orientation(int rank)
	{
		int sum = 0;
		std::array<int, 12> edges_orientation;

		for (int i = 0; i != 11; i++)
		{
			int res = rank % 2;
			edges_orientation[i] = res;
			sum += res;
			rank /= 2;
		}

		edges_orientation[11] = (2 - (sum % 2)) % 2;
		return edges_orientation;
	}

	static inline std::array<int, 8> unrank_corners_orientation(int rank)
	{
		int sum = 0;
		std::array<int, 8> corners_orientation;

		for (int i = 0; i != 7; i++)
		{
			int res = rank % 3;
			corners_orientation[i] = res;
			sum += res;
			rank /= 3;
		}

		corners_orientation[7] = (3 - (sum % 3)) % 3;
		return corners_orientation;
	}

	static inline std::array<int, 4> unrank_UD_Slice_phase1(int rank)
	{
		std::array<int, 4> positions;

		int max_p = 11;
		int res;

		for (int i = 4; i >= 1; i--)
		{
			int p = max_p;
			while ((res = binomialCoef(p, i)) > rank)
				--p;

			positions[i - 1] = p;
			rank -= res;
			max_p = p - 1;
		}

		return positions;
	}

	static inline std::array<EdgePos, 4> unrank_UD_Slice_phase2(int rank)
	{
		std::array<EdgePos, 4> edges_order;
		std::array<int, 4> lehmer_code;
		std::vector<int> available = {0, 1, 2, 3};
		static const std::array<EdgePos, 4> base_edges_order = {FR, FL, BR, BL};

		for (int i = 4; i >= 1; i--)
		{
			lehmer_code[4 - i] = rank / factorial[i - 1];
			rank %= factorial[i - 1];
		}

		for (int i = 0; i != 4; i++)
		{
			edges_order[i] = base_edges_order[available[lehmer_code[i]]];
			available.erase(available.begin() + lehmer_code[i]);
		}

		return edges_order;
	}

	static inline std::array<CornerPos, 8> unrank_corners_perm_coordinate(int rank)
	{
		std::vector<int> available = {UFR, UBR, UBL, UFL, DFR, DBR, DBL, DFL};
		std::array<CornerPos, 8> corners_order;
		std::array<int, 8> lehmer_code;

		for (int i = 8; i >= 1; i--)
		{
			lehmer_code[8 - i] = rank / factorial[i - 1];
			rank %= factorial[i - 1];
		}

		for (int i = 0; i != 8; i++)
		{
			corners_order[i] = (CornerPos)available[lehmer_code[i]];
			available.erase(available.begin() + lehmer_code[i]);
		}

		return corners_order;
	}

	static inline std::array<EdgePos, 8> unrank_UD_edge_perm_coordinate(int rank)
	{
		std::array<EdgePos, 8> edges_order;
		std::array<int, 8> lehmer_code;
		std::vector<int> available = {0, 1, 2, 3, 4, 5, 6, 7};
		static const std::array<EdgePos, 8> base_edges_order = {UR, UF, UL, UB, DR, DF, DL, DB};

		for (int i  = 8; i >= 1; i--)
		{
			lehmer_code[8 - i] = rank / factorial[i - 1];
			rank %= factorial[i - 1];
		}

		for (int i = 0; i != 8; i++)
		{
			edges_order[i] = base_edges_order[available[lehmer_code[i]]];
			available.erase(available.begin() + lehmer_code[i]);
		}

		return edges_order;
	}
}
