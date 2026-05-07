#pragma once
#include <random>
#include <string>
#include <raylib.h>
#include <array>

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

static inline Color get_color_by_face(Face face)
{
	static const std::array<Color, 6> color_by_face = {WHITE, YELLOW, RED, ORANGE, BLUE, GREEN};
	return color_by_face[face];
}

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

inline constexpr std::array<std::array<Face, 3>, 8> CornerToFace =
{{
	{{U, F, R}}, // UFR
	{{U, B, R}}, // UBR
	{{U, B, L}}, // UBL
	{{U, F, L}}, // UFL
	{{D, F, R}}, // DFR
	{{D, B, R}}, // DBR
	{{D, B, L}}, // DBL
	{{D, F, L}}  // DFL
}};

inline constexpr std::array<std::array<int, 3>, 8> CornerToFacelet =
{{
	{{8, 2, 0}}, // UFR: U8, F2, R0
	{{2, 0, 2}}, // UBR: U2, B0, R2
	{{0, 2, 0}}, // UBL: U0, B2, L0
	{{6, 0, 2}}, // UFL: U6, F0, L2

	{{2, 8, 6}}, // DFR: D2, F8, R6
	{{8, 6, 8}}, // DBR: D8, B6, R8
	{{6, 8, 6}}, // DBL: D6, B8, L6
	{{0, 6, 8}}  // DFL: D0, F6, L8
}};

struct Corner
{
	Corner(CornerPos pos, int orientation): pos(pos), orientation(orientation) {}
	Corner(): pos(UFR), orientation(0) {}
	CornerPos	pos;
	int			orientation;
};
