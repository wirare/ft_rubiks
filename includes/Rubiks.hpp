#pragma once

#include <array>
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
	Move();
	Face face;
	Modifier modifier;

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
	Corner();
	CornerPos pos;
	int orientation;
};

struct Edge
{
	Edge(EdgePos pos, int orientation): pos(pos), orientation(orientation) {}
	Edge();
	EdgePos pos;
	int orientation;
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

		void apply_move(const Move& move)
		{
			CornerRotDef CornerRotations = CornerRotDefTable[move.face];
			EdgeRotDef EdgeRotations = EdgeRotDefTable[move.face];
			std::array<int, 4> CornerTwistDeltas = CornerTwistDeltaTable[move.face];

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

		bool is_solved()
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

	private:
		using CornerRotDef = std::pair<std::array<CornerPos, 4>, std::array<CornerPos, 4>>;
		using EdgeRotDef = std::pair<std::array<EdgePos, 4>, std::array<EdgePos, 4>>;

		std::array<Corner, 8> Corners;
		std::array<Edge, 12> Edges;

		std::array<Corner, 8> CornersBuf;
		std::array<Edge, 12> EdgesBuf;

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
