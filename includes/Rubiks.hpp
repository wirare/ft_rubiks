#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

using Face = enum {
	U,
	D,
	F,
	B,
	R,
	L
};

using Modifier = enum {
	NONE = 1,
	TWICE = 2,
	COUNTER = 3
};

#define CASE_FACE_CHAR(x) case x: result += #x; break
struct Move
{
	Move(Face face, Modifier modifier): face(face), modifier(modifier) {}
	Move(): face(U), modifier(NONE) {}
	Face		face;
	Modifier	modifier;

	explicit operator std::string() const
	{
		std::string result;

		switch (face)
		{
			CASE_FACE_CHAR(U);
			CASE_FACE_CHAR(D);
			CASE_FACE_CHAR(L);
			CASE_FACE_CHAR(R);
			CASE_FACE_CHAR(F);
			CASE_FACE_CHAR(B);
		}

		switch (modifier)
		{
			case NONE: break;
			case COUNTER: result += "'"; break;
			case TWICE: result += "2"; break;
		}

		return result;
	}
};
#undef CASE_FACE_CHAR

using CornerPos = enum
{
	UFR,
	UBR,
	UBL,
	UFL,
	DFR,
	DBR,
	DBL,
	DFL
};

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

struct Corner
{
	Corner(CornerPos pos, int orientation): pos(pos), orientation(orientation) {}
	Corner(): pos(UFR), orientation(0) {}
	CornerPos	pos;
	int			orientation;
};

struct Edge
{
	Edge(EdgePos pos, int orientation): pos(pos), orientation(orientation) {}
	Edge(): pos(UR), orientation(0) {}
	EdgePos		pos;
	int			orientation;
};

class Rubiks
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

		Rubiks &operator=(const Rubiks& other)
		{
			Corners = other.Corners;
			Edges = other.Edges;
			return *this;
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

	private:
		using CornerRotDef	= std::pair<std::array<CornerPos, 4>, std::array<CornerPos, 4>>;
		using EdgeRotDef	= std::pair<std::array<EdgePos, 4>, std::array<EdgePos, 4>>;

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
