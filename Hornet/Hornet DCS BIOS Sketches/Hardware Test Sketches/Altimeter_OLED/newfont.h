// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Dialog_plain_10Bitmaps[] PROGMEM = {

	// Bitmap Data:
	0x00, // ' '
	0xAA,0x88, // '!'
	0xAA,0xA0, // '"'
	0x24,0x24,0x7E,0x28,0xFC,0x48,0x48, // '#'
	0x21,0xEA,0x38,0x38,0xAF,0x08, // '$'
	0xE4,0x52,0x2A,0x1F,0xE1,0x51,0x28,0x9C, // '%'
	0x30,0x48,0x40,0xB2,0x8A,0xCC,0x72, // '&'
	0xA8, // '''
	0x52,0x49,0x24,0x40, // '('
	0x91,0x24,0x94,0x80, // ')'
	0xA9,0xC7,0x2A, // '*'
	0x10,0x10,0x10,0xFE,0x10,0x10,0x10, // '+'
	0xA0, // ','
	0xE0, // '-'
	0x80, // '.'
	0x22,0x44,0x44,0x88, // '/'
	0x72,0x28,0xA2,0x8A,0x27,0x00, // '0'
	0xE0,0x82,0x08,0x20,0x8F,0x80, // '1'
	0x72,0x20,0x84,0x21,0x0F,0x80, // '2'
	0x72,0x20,0x9C,0x0A,0x27,0x00, // '3'
	0x10,0xC5,0x24,0xF8,0x41,0x00, // '4'
	0xF2,0x0F,0x02,0x08,0x2F,0x00, // '5'
	0x7B,0x08,0x3C,0x8A,0x27,0x00, // '6'
	0xF8,0x21,0x04,0x20,0x84,0x00, // '7'
	0x72,0x28,0x9C,0x8A,0x27,0x00, // '8'
	0x72,0x28,0x9E,0x08,0x6F,0x00, // '9'
	0x80,0x80, // ':'
	0x80,0xA0, // ';'
	0x04,0x73,0x01,0xC0,0x40, // '<'
	0xFC,0x03,0xF0, // '='
	0x80,0xE0,0x33,0x88,0x00, // '>'
	0xF0,0x88,0x84,0x01,0x00, // '?'
	0x3E,0x18,0x4C,0x0A,0x72,0x95,0xA7,0xCC,0x01,0x88,0x3C,0x00, // '@'
	0x10,0x28,0x28,0x44,0x7C,0x44,0x82, // 'A'
	0xF2,0x28,0xBC,0x8A,0x2F,0x00, // 'B'
	0x38,0x8A,0x04,0x08,0x08,0x8E,0x00, // 'C'
	0xF9,0x1A,0x14,0x28,0x51,0xBE,0x00, // 'D'
	0xFA,0x08,0x3E,0x82,0x0F,0x80, // 'E'
	0xF4,0x21,0xE8,0x42,0x00, // 'F'
	0x79,0x8A,0x04,0xE8,0x58,0x9E,0x00, // 'G'
	0x85,0x0A,0x17,0xE8,0x50,0xA1,0x00, // 'H'
	0xAA,0xA8, // 'I'
	0x22,0x22,0x22,0x22,0xC0, // 'J'
	0x89,0x22,0x86,0x0A,0x12,0x22,0x00, // 'K'
	0x82,0x08,0x20,0x82,0x0F,0x80, // 'L'
	0x82,0xC6,0xC6,0xAA,0xAA,0x92,0x82, // 'M'
	0x85,0x8A,0x95,0x29,0x51,0xA1,0x00, // 'N'
	0x79,0x9A,0x14,0x28,0x59,0x9E,0x00, // 'O'
	0xF2,0x28,0xBC,0x82,0x08,0x00, // 'P'
	0x79,0x9A,0x14,0x28,0x59,0x1C,0x04, // 'Q'
	0xF1,0x12,0x27,0x89,0x11,0x21,0x00, // 'R'
	0x72,0x28,0x1C,0x0A,0x27,0x00, // 'S'
	0xF8,0x82,0x08,0x20,0x82,0x00, // 'T'
	0x85,0x0A,0x14,0x28,0x50,0x9E,0x00, // 'U'
	0x82,0x82,0x44,0x44,0x28,0x28,0x10, // 'V'
	0x88,0xA2,0x25,0x51,0x54,0x55,0x08,0x82,0x20, // 'W'
	0xCC,0x90,0xC1,0x83,0x09,0x33,0x00, // 'X'
	0x82,0x44,0x28,0x10,0x10,0x10,0x10, // 'Y'
	0xFC,0x10,0x41,0x82,0x08,0x3F,0x00, // 'Z'
	0xD2,0x49,0x24,0xC0, // '['
	0x88,0x44,0x44,0x22, // '\'
	0xC9,0x24,0x92,0xC0, // ']'
	0x30,0x92,0x10, // '^'
	0xF8, // '_'
	0x88, // '`'
	0x70,0x27,0xA2,0xF8, // 'a'
	0x82,0x08,0x3C,0x8A,0x28,0xBC, // 'b'
	0x74,0x21,0x07,0x00, // 'c'
	0x08,0x20,0x9E,0x8A,0x28,0x9E, // 'd'
	0x72,0x2F,0xA0,0x78, // 'e'
	0x72,0x11,0xC4,0x21,0x08, // 'f'
	0x7A,0x28,0xA2,0x78,0x27,0x00, // 'g'
	0x82,0x08,0x3C,0x8A,0x28,0xA2, // 'h'
	0x82,0xAA, // 'i'
	0x40,0x24,0x92,0x58, // 'j'
	0x82,0x08,0x24,0xA3,0x0A,0x24, // 'k'
	0xAA,0xAA, // 'l'
	0xF7,0x22,0x28,0x8A,0x22,0x88,0x80, // 'm'
	0xF2,0x28,0xA2,0x88, // 'n'
	0x72,0x28,0xA2,0x70, // 'o'
	0xF2,0x28,0xA2,0xF2,0x08,0x00, // 'p'
	0x7A,0x28,0xA2,0x78,0x20,0x80, // 'q'
	0xE8,0x88,0x80, // 'r'
	0xF4,0x1C,0x2F,0x00, // 's'
	0x42,0x3C,0x84,0x21,0xC0, // 't'
	0x8A,0x28,0xA2,0x78, // 'u'
	0x8A,0x25,0x14,0x20, // 'v'
	0x92,0xAA,0xAA,0x44,0x44, // 'w'
	0x89,0x42,0x14,0x88, // 'x'
	0x8A,0x25,0x14,0x20,0x8C,0x00, // 'y'
	0xF0,0x88,0x8F,0x00, // 'z'
	0x31,0x08,0x4C,0x10,0x84,0x30, // '{'
	0xAA,0xAA,0xA0, // '|'
	0xC2,0x10,0x83,0x21,0x08,0xC0 // '}'
};
