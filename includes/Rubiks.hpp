#pragma once

#include "Algorithms.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <Common.hpp>
#include <Puzzle.hpp>
#include <Utility.hpp>

using EdgePos = enum
{
	UR,
	UF,
	UL,
	UB,
	DR,
	DF,
	DL,
	DB,
	FR,
	FL,
	BR,
	BL
};

struct Edge
{
	Edge(EdgePos pos, int orientation): pos(pos), orientation(orientation) {}
	Edge(): pos(UR), orientation(0) {}
	EdgePos		pos;
	int			orientation;
};

class Rubiks : public Puzzle
{
	public:
		Rubiks()
		{
			for (int i = 0; i != 8; i++)
 			{	
				Corners[i].pos = (CornerPos)i;
				Corners[i].orientation = 0;
			}

			for (int i = 0; i != 12; i++)
			{
				Edges[i].pos = (EdgePos)i;
				Edges[i].orientation = 0;
			}
		}

		Rubiks(const Rubiks& other)
		{
			Corners = other.Corners;
			Edges = other.Edges;
		}

		~Rubiks() {};

		Rubiks &operator=(const Rubiks& other)
		{
			Corners = other.Corners;
			Edges = other.Edges;
			return *this;
		}

		void solve()
		{
			auto phase1_result = Start_phase1_IDA_Star(*this);
			if (phase1_result.first == false)
				throw std::runtime_error("Couldnt solve phase 1\n");

			apply_move_vector(phase1_result.second);
			auto phase2_result = Start_phase2_IDA_Star(*this);
			if (phase2_result.first == false)
				throw std::runtime_error("Couldnt solve phase 2\n");

			std::cout << "Cube solved in " << phase1_result.second.size() + phase2_result.second.size() << " moves\n";
			for (Move &move : phase1_result.second)
				std::cout << static_cast<std::string>(move) << " ";
			for (Move &move : phase2_result.second)
				std::cout << static_cast<std::string>(move) << " ";
			std::cout << std::endl;
		}

		void solve_best()
		{
			auto result = Find_better_solution(*this);
			if (result.first == false)
				throw std::runtime_error("Couldnt solve cube\n");

			std::cout << "Cube solved in " << result.second.size() << " moves\n";
			for (Move &move : result.second)
				std::cout << static_cast<std::string>(move) << " ";
			std::cout << std::endl;
		}

		inline void apply_move(const Move& move)
		{
			CornerRotDef		CornerRotations		= CornerRotDefTable[move.face];
			EdgeRotDef			EdgeRotations		= EdgeRotDefTable[move.face];
			std::array<int, 4>	CornerTwistDeltas	= CornerTwistDeltaTable[move.face];

			for (int j = 0; j != move.modifier; j++)
			{
				CornersBuf = Corners;
				EdgesBuf = Edges;

				for (int i = 0; i != 4; i++)
				{
					CornersBuf[CornerRotations.second[i]] = Corners[CornerRotations.first[i]];
					EdgesBuf[EdgeRotations.second[i]] = Edges[EdgeRotations.first[i]];

					if (move.face == F || move.face == B)
						EdgesBuf[EdgeRotations.second[i]].orientation = !EdgesBuf[EdgeRotations.second[i]].orientation; 
					
					CornersBuf[CornerRotations.second[i]].orientation = (CornersBuf[CornerRotations.second[i]].orientation + CornerTwistDeltas[i]) % 3;
				}

				Corners = CornersBuf;
				Edges = EdgesBuf;
			}
		}

		void apply_move_vector(const std::vector<Move> &moves)
		{
			for (const Move &move : moves)
				apply_move(move);
		}

		bool is_solved() const
		{
			for (int i = 0; i != 8; i++)
			{
				if (Corners[i].pos != (CornerPos)i)
					return false;
				if (Corners[i].orientation != 0)
					return false;
			}

			for (int i = 0; i != 12; i++)
			{
				if (Edges[i].pos != (EdgePos)i)
					return false;
				if (Edges[i].orientation != 0)
					return false;
			}

			return true;
		}

		void apply_edge_orientation(const std::array<int, 12> &orientations)
		{
			for (int i = 0; i != 12; i++)
				Edges[i].orientation = orientations[i];
		}

		void apply_corner_orientation(const std::array<int, 8> &orientations)
		{
			for (int i = 0; i != 8; i++)
				Corners[i].orientation = orientations[i];
		}

		void set_8_corners(const std::array<CornerPos, 8> &pos)
		{
			for (int i = 0; i != 8; i++)
				Corners[i].pos = pos[i];
		}

		void set_4_edges(const std::array<int, 4> &pos)
		{
			std::vector<EdgePos> available_edges = {UR, UF, UL, UB, DR, DF, DL, DB};
			std::vector<EdgePos> middle_slice_edges = {BL, BR, FL, FR};

			for (int i = 0; i != 12; i++)
			{
				if (std::find(pos.begin(), pos.end(), i) != pos.end())
				{
					Edges[i].pos = middle_slice_edges.back();
					middle_slice_edges.pop_back();
				}
				else
				{
					Edges[i].pos = available_edges.back();
					available_edges.pop_back();
				}
			}
		}

		void set_8_edges(const std::array<EdgePos, 8> &pos)
		{
			for (int i = 0; i != 8; i++)
			{
				Edges[i].pos = pos[i];
			}
		}

		void set_4_edges_ordered(const std::array<EdgePos, 4> &pos)
		{
			for (int i = 0; i != 4; i++)
			{
				Edges[i + 8].pos = pos[i];
			}
		}

		const std::array<Corner, 8> &get_corners() const { return Corners; }
		const std::array<Edge, 12> &get_edges() const { return Edges; }
		std::size_t get_moveset_size() const { return moveset_size; }

		const Move& move_generator(int i) const { return get_move(i); }

		#define CASE_FACE(x) case *#x: tmp.face = x; break
		std::vector<Move> parse_moves(const std::string& moves_str) const
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

		CubeType get_type() const { return CLASSIC; }

		void print_move_vector(const std::vector<Move> &moves) const
		{
			for (const Move &move : moves)
				std::cout << std::string(move) << " ";
			std::cout << std::endl;
		}

		Rubiks inverse() const
		{
			Rubiks inv;

			for (int i = 0; i != 8; i++)
			{
				CornerPos cubie = Corners[i].pos;

				inv.Corners[cubie].pos = static_cast<CornerPos>(i);
				inv.Corners[cubie].orientation = (3 - Corners[i].orientation) % 3;
			}

			for (int i = 0; i != 12; i++)
			{
				EdgePos cubie = Edges[i].pos;

				inv.Edges[cubie].pos = static_cast<EdgePos>(i);
				inv.Edges[cubie].orientation = (2 - Edges[i].orientation) % 2;
			}

			return inv;
		}

		friend Rubiks compose(const Rubiks &A, const Rubiks &B);

	private:
		using CornerRotDef	= std::pair<std::array<CornerPos, 4>, std::array<CornerPos, 4>>;
		using EdgeRotDef	= std::pair<std::array<EdgePos, 4>, std::array<EdgePos, 4>>;

		const int moveset_size = 18;
		//const int reduce_moveset_size = 10;

		std::array<Corner, 8>	Corners;
		std::array<Edge, 12>	Edges;

		std::array<Corner, 8>	CornersBuf;
		std::array<Edge, 12>	EdgesBuf;

		const std::vector<CornerRotDef> CornerRotDefTable = {
			{{ UFR, UFL, UBL, UBR }, { UFL, UBL, UBR, UFR }}, //U
			{{ DFR, DBR, DBL, DFL }, { DBR, DBL, DFL, DFR }}, //D
			{{ UFR, DFR, DFL, UFL }, { DFR, DFL, UFL, UFR }}, //F
			{{ UBR, UBL, DBL, DBR }, { UBL, DBL, DBR, UBR }}, //B
			{{ UFR, UBR, DBR, DFR }, { UBR, DBR, DFR, UFR }}, //R
			{{ UFL, DFL, DBL, UBL }, { DFL, DBL, UBL, UFL }}  //L
		};

		const std::vector<EdgeRotDef> EdgeRotDefTable = {
			{{ UF, UL, UB, UR }, { UL, UB, UR, UF }}, //U
			{{ DF, DR, DB, DL }, { DR, DB, DL, DF }}, //D
			{{ UF, FR, DF, FL }, { FR, DF, FL, UF }}, //F
			{{ UB, BL, DB, BR }, { BL, DB, BR, UB }}, //B
			{{ FR, UR, BR, DR }, { UR, BR, DR, FR }}, //R
			{{ FL, DL, BL, UL }, { DL, BL, UL, FL }}  //L
		};

		const std::vector<std::array<int, 4>> CornerTwistDeltaTable = {
			{0, 0, 0, 0}, //U
			{0, 0, 0, 0}, //D
			{2, 1, 2, 1}, //F
			{1, 2, 1, 2}, //B
			{1, 2, 1, 2}, //R
			{2, 1, 2, 1}  //L
		};
};

inline Rubiks compose(const Rubiks &A, const Rubiks &B)
{
	Rubiks C;

	for (int i = 0; i != 8; i++)
	{
		C.Corners[i].pos = A.Corners[B.Corners[i].pos].pos;
		C.Corners[i].orientation = (A.Corners[B.Corners[i].pos].orientation + B.Corners[i].orientation) % 3;
	}
	
	for (int i = 0; i != 12; i++)
	{
		C.Edges[i].pos = A.Edges[B.Edges[i].pos].pos;
		C.Edges[i].orientation = (A.Edges[B.Edges[i].pos].orientation + B.Edges[i].orientation) % 2;
	}

	return C;
}
