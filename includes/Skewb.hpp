#pragma once

#include "Algorithms.hpp"
#include "Puzzle.hpp"
#include "Utility.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <Common.hpp>
#include <array>
#include <vector>

using CenterPos = enum
{
	CU,
	CD,
	CF,
	CB,
	CR,
	CL
};

struct Center
{
	Center(CenterPos pos): pos(pos) {}
	Center(): pos(CU) {}
	CenterPos	pos;
};



class Skewb : public Puzzle
{
	public:
		Skewb()
		{
			for (int i = 0; i != 8; i++)
			{
				Corners[i].pos = (CornerPos)i;
				Corners[i].orientation = 0;
			}

			for (int i = 0; i != 6; i++)
				Centers[i] = (CenterPos)i;
		}

		Skewb(const Skewb& other)
		{
			Corners = other.Corners;
			Centers = other.Centers;
		}

		~Skewb() {};

		Skewb &operator=(const Skewb& other)
		{
			Corners = other.Corners;
			Centers = other.Centers;
			return *this;
		}

		void solve()
		{
			auto result = Start_skewb_IDA_Star(*this);
			if (result.first == false)
				throw std::runtime_error("Couldnt solve skewb\n");

			std::cout << "Cube solved in " << result.second.size() << " moves\n";
			print_move_vector(result.second);
		}

		const std::array<Corner, 8> &get_corners() const { return Corners; }
		const std::array<Center, 6> &get_centers() const { return Centers; }

		bool is_solved() const
		{
			for (int i = 0; i != 8; i++)
			{
				if (Corners[i].pos != (CornerPos)i)
					return false;
				if (Corners[i].orientation != 0)
					return false;
			}

			for (int i = 0; i != 6; i++)
			{
				if (Centers[i].pos != (CenterPos)i)
					return false;
			}

			return true;
		}

		inline int face_to_skewb_table_index(Face face)
		{
			switch (face)
			{
				case U: return 0;
				case B: return 1;
				case R: return 2;
				case L: return 3;
				default:
						throw std::invalid_argument("Invalid Skewb move face");
			}
		}

		inline void apply_move(const Move& move)
		{
			int							TableIndex			=	face_to_skewb_table_index(move.face);
			CornerRotDef				CornerRotations		=	CornerRotDefTable[TableIndex];
			CenterRotDef				CenterRotations		=	CenterRotDefTable[TableIndex];
			std::array<CornerPos, 4>	AffectedCorners		=	AffectedCornerTable[TableIndex];
			int							CornerTwistDelta	=	CornerTwistDeltaTable[TableIndex];

			int apply_number = move.modifier == NONE ? 1 : 2;

			for (int j = 0; j != apply_number; j++)
			{
				CornersBuf = Corners;
				CentersBuf = Centers;

				for (int i = 0; i != 3; i++)
				{
					CornersBuf[CornerRotations.second[i]] = Corners[CornerRotations.first[i]];
					CentersBuf[CenterRotations.second[i]] = Centers[CenterRotations.first[i]];
				}

				for (int i = 0; i != 4; i++)
					CornersBuf[AffectedCorners[i]].orientation = (CornersBuf[AffectedCorners[i]].orientation + CornerTwistDelta) % 3;

				Corners = CornersBuf;
				Centers = CentersBuf;
			}
		}

		void apply_move_vector(const std::vector<Move> &moves)
		{
			for (const Move &move : moves)
			{
				apply_move(move);
				/*
				if (print)
				{
					print_corners_pos();
					print_centers_pos();
				}
				*/
			}
		}

		void set_corner_orientation(const std::array<int, 8> &corners_orientations)
		{
			for (int i = 0; i != 8; i++)
				Corners[i].orientation = corners_orientations[i];
		}

		void set_centers(const std::array<CenterPos, 6> &centers_pos)
		{
			for (int i = 0; i != 6; i++)
				Centers[i].pos = centers_pos[i];
		}

		void set_corner_perm(const std::array<CornerPos, 8> &corners_pos)
		{
			for (int i = 0; i != 8; i++)
				Corners[i].pos = corners_pos[i];
		}
		
		#define CASE_PIECE(x) case x: return std::string(#x)
		std::string corners_pos_to_str(CornerPos pos) const
		{
			switch (pos)
			{
				CASE_PIECE(UBR);
				CASE_PIECE(UFR);
				CASE_PIECE(UFL);
				CASE_PIECE(UBL);
				CASE_PIECE(DBR);
				CASE_PIECE(DFR);
				CASE_PIECE(DFL);
				CASE_PIECE(DBL);
			}
		}

		std::string centers_pos_to_str(CenterPos pos) const
		{
			switch (pos)
			{
				CASE_PIECE(CU);
				CASE_PIECE(CD);
				CASE_PIECE(CF);
				CASE_PIECE(CB);
				CASE_PIECE(CL);
				CASE_PIECE(CR);
			}
		}

		void print_corners_pos() const
		{
			for (int i = 0; i != 8; i++)
				std::cout << "Corner at pos " << corners_pos_to_str((CornerPos)i) << " is corner " << corners_pos_to_str(Corners[i].pos) << " with orientation " << Corners[i].orientation << std::endl;
		}

		void print_centers_pos() const 
		{
			for (int i = 0; i != 6; i++)
				std::cout << "Center at pos " << centers_pos_to_str((CenterPos)i) << " is center " << centers_pos_to_str(Centers[i].pos) << std::endl;
		}

		const Move& move_generator(int i) const { return get_skewb_move(i); }
		std::size_t get_moveset_size() const { return moveset_size; }

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
					CASE_FACE(L);
					CASE_FACE(R);
					CASE_FACE(B);
					default:
						throw std::invalid_argument("Error in the shuffle string: Wrong face");
				}

				if (token.length() == 2)
				{
					switch (token[1])
					{
						case '\'': tmp.modifier = TWICE; break;
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

		CubeType get_type() const { return SKEWB; }

		#define CASE_FACE_CHAR(x) case x: result += #x; break
		static inline std::string print_move(const Move &move)
		{
			std::string result;

			switch (move.face)
			{
				CASE_FACE_CHAR(U);
				CASE_FACE_CHAR(L);
				CASE_FACE_CHAR(R);
				CASE_FACE_CHAR(B);
				default:
				__builtin_unreachable();
			}

			switch (move.modifier)
			{
				case NONE: break;
				case TWICE: result += "'"; break;
				default:
							__builtin_unreachable();
			}

			return result;
		}
		#undef CASE_FACE_CHAR

		void print_move_vector(const std::vector<Move> &moves) const
		{
			for (const Move &move : moves)
				std::cout << print_move(move) << " ";
			std::cout << std::endl;
		}

	private:
		using CenterRotDef	= std::pair<std::array<CenterPos, 3>, std::array<CenterPos, 3>>;
		using CornerRotDef	= std::pair<std::array<CornerPos, 3>, std::array<CornerPos, 3>>;

		const std::size_t moveset_size = 8;

		std::array<Corner, 8> Corners;
		std::array<Center, 6> Centers;

		std::array<Corner, 8> CornersBuf;
		std::array<Center, 6> CentersBuf;

		const std::vector<CenterRotDef> CenterRotDefTable = {
			{{ CU, CL, CB }, { CL, CB, CU }}, //U
			{{ CB, CL, CD }, { CL, CD, CB }}, //B
			{{ CR, CB, CD }, { CB, CD, CR }}, //R
			{{ CF, CL, CD }, { CD, CF, CL }}, //L
		};

		const std::vector<CornerRotDef> CornerRotDefTable = {
			{{ UFL, DBL, UBR }, { DBL, UBR, UFL }}, //U
			{{ UBL, DFL, DBR }, { DFL, DBR, UBL }}, //B
			{{ UBR, DBL, DFR }, { DBL, DFR, UBR }}, //R
			{{ UFL, DFR, DBL }, { DFR, DBL, UFL }}, //L
		};

		const std::vector<std::array<CornerPos, 4>> AffectedCornerTable = {
			{ UBL, UFL, DBL, UBR }, //U
			{ DBL, UBL, DFL, DBR }, //B
			{ DBR, UBR, DBL, DFR }, //R
			{ DFL, UFL, DFR, DBL }, //L
		};

		const std::array<int, 4> CornerTwistDeltaTable = {
			2, //U
			1, //B
			2, //R
			2, //L
		};
};
