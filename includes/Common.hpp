#pragma once
#include <string>

using CubeType = enum {
	CLASSIC,
	SKEWB,
};

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

struct Corner
{
	Corner(CornerPos pos, int orientation): pos(pos), orientation(orientation) {}
	Corner(): pos(UFR), orientation(0) {}
	CornerPos	pos;
	int			orientation;
};
