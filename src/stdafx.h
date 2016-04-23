// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <PlayrixEngine.h>
enum TowerType {
	NORMAL,
	SPLASH,
	SLOW,
	DECAY,
	BASH,
	DESTROY,
	EMPTY
};

typedef struct {
	IPoint _a0;
	IPoint _a45;
	IPoint _a90;
	IPoint _a135;
	IPoint _a180;
	IPoint _a225;
	IPoint _a270;
	IPoint _a315;
}AnimAngles;



typedef struct {
	float angleStart;
	float angleFinish;
} Sector;