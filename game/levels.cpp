#include "game/levels.h"

using namespace hfh3;

/* Note this definition uses C99 designated initializers for struct members.
 * This is not a part of the C++ standard, but allowed by g++
 */
Levels::Levels()
: Array({
	// Level 1:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 768, 1024,  128,  128},
			{ 384, 1280,  128,  128},
		},
	},
	// Level 2:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 256,  768,  192,  192},
			{ 768,  768,  192,  192},
			{ 256, 1024,  192,  192},
			{ 768, 1024,  192,  192},
		},
	},
	// Level 3:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 256,  640,  128,  128},
			{ 256,  960,  128,  128},
			{ 256, 1280,  128,  128},
			{ 768,  640,  128,  128},
			{ 768,  960,  128,  128},
			{ 768, 1280,  128,  128},
		},
	},
	// Level 4:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 224,  880,  192,  192},
			{ 880,  224,  192,  192},
			{ 560, 1120,  192,  192},
		},
	},
	// Level 5:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512,  512,  192,  192},
			{ 512, 1024,  192,  192},
		},
	},
	// Level 6:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512, 1024,  256,  256},
		},
	},
	// Level 7:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512,  864,   96,   96},
			{ 672,  864,   96,   96},
			{ 672, 1024,   96,   96},
			{ 672, 1184,   96,   96},
			{ 512, 1184,   96,   96},
			{ 352, 1184,   96,   96},
			{ 352, 1024,   96,   96},
			{ 352,  864,   96,   96},
		},
	},
	// Level 8:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 768, 1024,  128,  128},
			{ 256, 1024,  128,  128},
			{ 512, 1280,  128,  128},
			{ 512,  768,  128,  128},
			{ 704, 1216,  128,  128},
			{ 320, 1216,  128,  128},
			{ 704,  832,  128,  128},
			{ 320,  832,  128,  128},
		},
	},
	// Level 9:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512,  512,  224,  224},
		},
	},
	// Level 10:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512, 1024,  384,  384},
		},
	},
	// Level 11:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 240, 1024,   96,   96},
			{ 400, 1024,   96,   96},
			{ 560, 1024,   96,   96},
			{ 720, 1024,   96,   96},
			{ 880, 1024,   96,   96},
		},
	},
	// Level 12:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 400,  848,  224,  192},
			{ 640, 1120,  160,  128},
		},
	},
	// Level 13:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 384,  128,  160,  160},
			{ 640,  128,  160,  160},
			{ 896,  128,  160,  160},
			{ 384,  384,  160,  160},
			{ 640,  384,  160,  160},
			{ 896,  384,  160,  160},
			{ 384,  640,  160,  160},
			{ 640,  640,  160,  160},
			{ 896,  640,  160,  160},
			{ 384,  896,  160,  160},
			{ 640,  896,  160,  160},
			{ 896,  896,  160,  160},
			{ 384, 1152,  160,  160},
			{ 640, 1152,  160,  160},
			{ 896, 1152,  160,  160},
			{ 384, 1408,  160,  160},
			{ 640, 1408,  160,  160},
			{ 896, 1408,  160,  160},
			{ 384, 1664,  160,  160},
			{ 640, 1664,  160,  160},
			{ 896, 1664,  160,  160},
			{ 384, 1920,  160,  160},
			{ 640, 1920,  160,  160},
			{ 896, 1920,  160,  160},
		},
	},
	// Level 14:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512, 1024,  768,  768},
		},
	},
	// Level 15:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512,  768,  224,  224},
			{ 768,  768,  224,  224},
			{ 768, 1024,  224,  224},
			{ 768, 1280,  224,  224},
			{ 512, 1280,  224,  224},
			{ 256, 1280,  224,  224},
			{ 256, 1024,  224,  224},
			{ 256,  768,  224,  224},
		},
	},
	// Level 16:
	{
		.playerStarts = {
			{ { 496, 1024}, Direction::West},
			{ { 528, 1024}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 256,  320,  128,  128},
			{ 256,  640,  128,  128},
			{ 256,  960,  128,  128},
			{ 256, 1280,  128,  128},
			{ 256, 1600,  128,  128},
			{ 768,  320,  128,  128},
			{ 768,  640,  128,  128},
			{ 768,  960,  128,  128},
			{ 768, 1280,  128,  128},
			{ 768, 1600,  128,  128},
		},
	},
	// Level 17:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512,  928,  576,  576},
		},
	},
	// Level 18:
	{
		.playerStarts = {
			{ { 496, 1152}, Direction::West},
			{ { 528, 1152}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 256,  768,   96,   96},
			{ 384,  768,   96,   96},
			{ 512,  768,   96,   96},
			{ 640,  768,   96,   96},
			{ 768,  768,   96,   96},
			{ 256, 1024,   96,   96},
			{ 384, 1024,   96,   96},
			{ 512, 1024,   96,   96},
			{ 640, 1024,   96,   96},
			{ 768, 1024,   96,   96},
			{ 256, 1280,   96,   96},
			{ 384, 1280,   96,   96},
			{ 512, 1280,   96,   96},
			{ 640, 1280,   96,   96},
			{ 768, 1280,   96,   96},
			{ 256, 1536,   96,   96},
			{ 384, 1536,   96,   96},
			{ 512, 1536,   96,   96},
			{ 640, 1536,   96,   96},
			{ 768, 1536,   96,   96},
		},
	},
	// Level 19:
	{
		.playerStarts = {
			{ { 496,  800}, Direction::West},
			{ { 528,  800}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 544,  224,  128,  128},
			{ 256,  352,  128,  128},
			{ 352,  608,  128,  128},
			{ 720,  640,  128,  128},
			{ 736, 1056,  128,  128},
			{ 480, 1120,  128,  128},
			{ 240, 1440,  128,  128},
		},
	},
	// Level 20:
	{
		.playerStarts = {
			{ { 496, 1536}, Direction::West},
			{ { 528, 1536}, Direction::East},
		},
		.enemies = {},
		.fortresses = {
			{ 512, 1024,  576,  576},
		},
	},

})
{}