#pragma once
#include <random>
#include <string>
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

using Lang = enum {
	LANG_FR,
	LANG_EN
};

static Lang language_g = LANG_EN;

#define CASE_FACE_CHAR(x) case x: result += #x; break
#define CASE_FACE_STR(x, str) case x: result += str; break
#define CASE_MOD_STR(mod, str) case mod: result += str; break
struct Move
{
	Move(Face face, Modifier modifier): face(face), modifier(modifier) {}
	bool operator==(const Move &other) const = default;
	Move(): face(U), modifier(NONE) {}
	Face		face;
	Modifier	modifier;
	bool		readable = false;


	explicit operator std::string() const
	{
		std::string result;

		if (!readable)
		{
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
		}
		else
		{
			switch (language_g)
			{
				case LANG_FR:
					{
						result += "Tourner la face ";
						switch (face)
						{
							CASE_FACE_STR(U, "du dessus ");
							CASE_FACE_STR(D, "du dessous ");
							CASE_FACE_STR(L, "de gauche ");
							CASE_FACE_STR(R, "de droite ");
							CASE_FACE_STR(F, "frontale ");
							CASE_FACE_STR(B, "arriere ");
						}

						switch (modifier)
						{
							CASE_MOD_STR(NONE, "dans le sens horaire");
							CASE_MOD_STR(COUNTER, "dans le sens anti-horaire");
							CASE_MOD_STR(TWICE, "deux fois");
						}
					}
				case LANG_EN:
					{
						result += "Rotate the ";
						switch (face)
						{
							CASE_FACE_STR(U, "top ");
							CASE_FACE_STR(D, "bottom ");
							CASE_FACE_STR(L, "left ");
							CASE_FACE_STR(R, "right ");
							CASE_FACE_STR(F, "front ");
							CASE_FACE_STR(B, "back ");
						}

						result += "face ";

						switch (modifier)
						{
							CASE_MOD_STR(NONE, "clockwise");
							CASE_MOD_STR(COUNTER, "counter-clockwise");
							CASE_MOD_STR(TWICE, "twice");
						}
					}
			}
			result += "\n";
		}


		return result;
	}

	inline void set_readable(bool set)
	{
		readable = set;
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
