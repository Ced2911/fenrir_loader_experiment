#pragma once
// all notes are 8bit with 2 loops

// a4
// 440hz
const unsigned char note_a4[200] = {
	0x00, 0x07, 0x0F, 0x17, 0x1F, 0x27, 0x2E, 0x35, 0x3D, 0x43, 0x4A, 0x50,
	0x56, 0x5C, 0x61, 0x66, 0x6B, 0x6F, 0x72, 0x75, 0x78, 0x7A, 0x7C, 0x7D,
	0x7E, 0x7E, 0x7E, 0x7E, 0x7C, 0x7B, 0x78, 0x76, 0x73, 0x6F, 0x6B, 0x67,
	0x62, 0x5D, 0x57, 0x51, 0x4B, 0x44, 0x3D, 0x36, 0x2F, 0x28, 0x20, 0x18,
	0x10, 0x08, 0x00, 0xF9, 0xF2, 0xEA, 0xE2, 0xDA, 0xD3, 0xCB, 0xC4, 0xBD,
	0xB7, 0xB0, 0xAA, 0xA5, 0x9F, 0x9A, 0x96, 0x92, 0x8E, 0x8B, 0x88, 0x86,
	0x84, 0x83, 0x82, 0x82, 0x82, 0x82, 0x83, 0x85, 0x87, 0x8A, 0x8D, 0x90,
	0x94, 0x99, 0x9E, 0xA3, 0xA8, 0xAE, 0xB5, 0xBB, 0xC2, 0xC9, 0xD0, 0xD8,
	0xDF, 0xE7, 0xEF, 0xF7, 0xFF, 0x06, 0x0E, 0x15, 0x1D, 0x25, 0x2C, 0x34,
	0x3B, 0x42, 0x49, 0x4F, 0x55, 0x5B, 0x60, 0x65, 0x6A, 0x6E, 0x71, 0x75,
	0x78, 0x7A, 0x7C, 0x7D, 0x7E, 0x7E, 0x7E, 0x7E, 0x7D, 0x7B, 0x79, 0x76,
	0x73, 0x70, 0x6C, 0x68, 0x63, 0x5E, 0x58, 0x52, 0x4C, 0x46, 0x3F, 0x38,
	0x31, 0x29, 0x22, 0x1A, 0x12, 0x0A, 0x02, 0xFB, 0xF3, 0xEB, 0xE4, 0xDC,
	0xD4, 0xCD, 0xC6, 0xBF, 0xB8, 0xB2, 0xAC, 0xA6, 0xA1, 0x9C, 0x97, 0x93,
	0x8F, 0x8C, 0x89, 0x86, 0x84, 0x83, 0x82, 0x82, 0x82, 0x82, 0x83, 0x85,
	0x87, 0x89, 0x8C, 0x90, 0x94, 0x98, 0x9D, 0xA2, 0xA7, 0xAD, 0xB3, 0xBA,
	0xC0, 0xC7, 0xCF, 0xD6, 0xDD, 0xE5, 0xED, 0xF5};

const unsigned char note_a4_square[200] = {
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const uint8_t sn_noise_per[256]  = {
	0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F,
	0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF,
	0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F,
	0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,
	0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F,
	0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF,
	0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 0x7F,
	0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
	0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF,
	0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F,
	0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF,
	0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF,
	0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF,
	0xFF, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F,
	0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0x7F,
	0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF,
	0x7F, 0x7F, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
	0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF,
	0x7F, 0x7F, 0x7F, 0xFF
};


// c#4
// 277.18hz
const unsigned char note_c_d_4[318] = {
	0x00, 0x05, 0x0A, 0x0F, 0x13, 0x18, 0x1D, 0x22, 0x27, 0x2C, 0x30, 0x35,
	0x39, 0x3E, 0x42, 0x46, 0x4B, 0x4F, 0x52, 0x56, 0x5A, 0x5D, 0x60, 0x64,
	0x67, 0x69, 0x6C, 0x6F, 0x71, 0x73, 0x75, 0x77, 0x79, 0x7A, 0x7B, 0x7C,
	0x7D, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7D, 0x7D, 0x7C, 0x7B, 0x79,
	0x78, 0x76, 0x74, 0x72, 0x70, 0x6E, 0x6B, 0x68, 0x65, 0x62, 0x5F, 0x5C,
	0x58, 0x54, 0x51, 0x4D, 0x49, 0x45, 0x40, 0x3C, 0x37, 0x33, 0x2E, 0x2A,
	0x25, 0x20, 0x1B, 0x16, 0x11, 0x0C, 0x07, 0x02, 0xFE, 0xF9, 0xF4, 0xEF,
	0xEA, 0xE5, 0xE1, 0xDC, 0xD7, 0xD2, 0xCE, 0xC9, 0xC5, 0xC0, 0xBC, 0xB8,
	0xB4, 0xB0, 0xAC, 0xA8, 0xA5, 0xA1, 0x9E, 0x9B, 0x98, 0x95, 0x93, 0x90,
	0x8E, 0x8C, 0x8A, 0x88, 0x87, 0x85, 0x84, 0x83, 0x83, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x89, 0x8B, 0x8D, 0x8F,
	0x91, 0x94, 0x96, 0x99, 0x9C, 0x9F, 0xA2, 0xA6, 0xAA, 0xAD, 0xB1, 0xB5,
	0xB9, 0xBD, 0xC2, 0xC6, 0xCB, 0xCF, 0xD4, 0xD9, 0xDD, 0xE2, 0xE7, 0xEC,
	0xF1, 0xF6, 0xFB, 0x00, 0x04, 0x09, 0x0E, 0x13, 0x18, 0x1D, 0x22, 0x26,
	0x2B, 0x30, 0x34, 0x39, 0x3D, 0x42, 0x46, 0x4A, 0x4E, 0x52, 0x56, 0x59,
	0x5D, 0x60, 0x63, 0x66, 0x69, 0x6C, 0x6E, 0x71, 0x73, 0x75, 0x77, 0x78,
	0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7D,
	0x7C, 0x7B, 0x7A, 0x78, 0x76, 0x74, 0x72, 0x70, 0x6E, 0x6B, 0x69, 0x66,
	0x63, 0x5F, 0x5C, 0x58, 0x55, 0x51, 0x4D, 0x49, 0x45, 0x41, 0x3C, 0x38,
	0x33, 0x2F, 0x2A, 0x25, 0x20, 0x1C, 0x17, 0x12, 0x0D, 0x08, 0x03, 0xFF,
	0xFA, 0xF5, 0xF0, 0xEB, 0xE6, 0xE1, 0xDC, 0xD7, 0xD3, 0xCE, 0xC9, 0xC5,
	0xC1, 0xBC, 0xB8, 0xB4, 0xB0, 0xAC, 0xA9, 0xA5, 0xA2, 0x9E, 0x9B, 0x98,
	0x96, 0x93, 0x90, 0x8E, 0x8C, 0x8A, 0x88, 0x87, 0x86, 0x84, 0x83, 0x83,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x89,
	0x8A, 0x8C, 0x8F, 0x91, 0x93, 0x96, 0x99, 0x9C, 0x9F, 0xA2, 0xA6, 0xA9,
	0xAD, 0xB1, 0xB5, 0xB9, 0xBD, 0xC1, 0xC6, 0xCA, 0xCF, 0xD3, 0xD8, 0xDD,
	0xE2, 0xE7, 0xEC, 0xF0, 0xF5, 0xFA};
