/*!
\file
\brief Основной файл
*/

#include <iostream>
#include <map>
#include <string>
#include <utility>  
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <string.h>
#include <stdlib.h>     
using namespace std;

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef long unsigned int	ulong;
typedef unsigned int uint;


// padding
unsigned char	PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


void encode(unsigned char *output, ulong *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j]		= (unsigned char) (input[i] & 0xFF);
		output[j + 1]	= (unsigned char) ((input[i] >> 8)  & 0xFF);
		output[j + 2]	= (unsigned char) ((input[i] >> 16)  & 0xFF);
		output[j + 3]	= (unsigned char) ((input[i] >> 24)  & 0xFF);
	}
}



// ------------------------------------------------------------
/** 
	\brief кодирование ulong в char с обратным порядком бит
	\param [out] output результат
	\param [in] input исходный ulong
	\param [in] len размер
*/
void encode_rev(unsigned char *output, ulong *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j + 3]	= (unsigned char) (input[i] & 0xFF);
		output[j + 2]	= (unsigned char) ((input[i] >> 8)  & 0xFF);
		output[j + 1]	= (unsigned char) ((input[i] >> 16)  & 0xFF);
		output[j + 0]	= (unsigned char) ((input[i] >> 24)  & 0xFF);
	}
}


// ------------------------------------------------------------
/** 
	\brief декодирование char в ulong
*/
void decode(ulong *output, unsigned char *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((ulong) input[j]) | (((ulong) input[j + 1]) << 8) |
		(((ulong) input[j + 2]) << 16) | (((ulong) input[j + 3]) << 24);
}



// ------------------------------------------------------------
// декодирование char в long в обратном порядке бит
//
/** 
	\brief декодирование char в ulong в обратном порядке бит
	\param [out] output результат
	\param [in] input исходный ulong
	\param [in] len размер
*/
void decode_rev(ulong *output, unsigned char *input, int len)
{
	int		i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((ulong) input[j + 3]) | (((ulong) input[j + 2]) << 8) |
		(((ulong) input[j + 1]) << 16) | (((ulong) input[j]) << 24);
}

/**
	\brief Структура блока
*/
typedef struct _ulong64
{
	ulong	l;
	ulong	h;
}
ulong64;

ulong64 key0;

/**
	Перестановка 1
	\param [inout] v блок 
	\return блок, проведенный через перестановку
*/
ulong64 pc1(ulong64 v)
{
	uchar	p1[] = {
					60, 52, 44, 36, 59, 51, 43,
					35, 27, 19, 11,  3, 58, 50,
					42, 34, 26, 18, 10,  2, 57,
					49, 41, 33, 25, 17,  9,  1};

	uchar	p2[] = {
					28, 20, 12,  4, 61, 53, 45,
					37, 29, 21, 13,  5, 62, 54,
					46, 38, 30, 22, 14,  6, 63,
					55, 47, 39, 31, 23, 15,  7};
	ulong64	r = {0, 0};
	ulong	t;
	uchar	s;
	int		i;

	// 28
	for (i = 27; i >= 0; i--)
	{
		t = v.l;
		s = p1[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.l |= ((t >> s) & 1) << i;
	}
	// 28
	for (i = 27; i >= 0; i--)
	{
		t = v.l;
		s = p2[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.h |= ((t >> s) & 1) << i;
	}
	return r;
}

/**
	Перестановка 2
	\param [inout] v блок 
*/
ulong64 pc2(ulong64 v)
{
	uchar	p1[] = {
					24, 27, 20,  6, 14, 10,
					 3, 22,  0, 17,  7, 12,
					 8, 23, 11,  5, 16, 26,
					 1,  9, 19, 25,  4, 15,
					54, 43, 36, 29, 49, 40,
					48, 30};
	uchar	p2[] = {
					52, 44, 37, 33,
					46, 35, 50, 41, 28, 53,
					51, 55, 32, 45, 39, 42};

	ulong64	r = {0, 0};
	ulong	t;
	uchar	s;
	int		i;

	// 32
	for (i = 31; i >= 0; i--)
	{
		t = v.l;
		s = p1[i];

		if (s > 27)
		{
			t = v.h;
			s -= 28;
		}
		r.l |= ((t >> s) & 1) << i;
	}
	// 16
	for (i = 15; i >= 0; i--)
	{
		t = v.l;
		s = p2[i];

		if (s > 27)
		{
			t = v.h;
			s -= 28;
		}
		r.h |= ((t >> s) & 1) << i;
	}
	return r;
}

/**
	\brief Начальная перестановка
	\param [inout] v блок 
*/
ulong64 ip(ulong64 v)
{
	uchar	p1[] = {
					57, 49, 41, 33, 25, 17,  9,  1,
					59, 51, 43, 35, 27, 19, 11,  3,
					61, 53, 45, 37, 29, 21, 13,  5,
					63, 55, 47, 39, 31, 23, 15,  7};
	uchar	p2[] = {
					56, 48, 40, 32, 24, 16,  8,  0,
					58, 50, 42, 34, 26, 18, 10,  2,
					60, 52, 44, 36, 28, 20, 12,  4,
					62, 54, 46, 38, 30, 22, 14,  6};
	ulong64	r = {0, 0};
	ulong	t;
	uchar	s;
	int		i;

	// 32
	for (i = 31; i >= 0; i--)
	{
		t = v.l;
		s = p1[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.l |= ((t >> s) & 1) << i;
	}
	// 32
	for (i = 31; i >= 0; i--)
	{
		t = v.l;
		s = p2[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.h |= ((t >> s) & 1) << i;
	}
	return r;
}

/** 
	\brief Обратная к начальной перестановка
	\param [inout] v блок 
*/
ulong64 rip(ulong64 v)
{
	uchar	p1[] = {
					39,  7, 47, 15, 55, 23, 63, 31,
					38,  6, 46, 14, 54, 22, 62, 30,
					37,  5, 45, 13, 53, 21, 61, 29,
					36,  4, 44, 12, 52, 20, 60, 28};
	uchar	p2[] = {
					35,  3, 43, 11, 51, 19, 59, 27,
					34,  2, 42, 10, 50, 18, 58, 26,
					33,  1, 41,  9, 49, 17, 57, 25,
					32,  0, 40,  8, 48, 16, 56, 24};
	ulong64	r = {0, 0};
	ulong	t;
	uchar	s;
	int		i;

	// 32
	for (i = 31; i >= 0; i--)
	{
		t = v.l;
		s = p1[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.l |= ((t >> s) & 1) << i;
	}
	// 32
	for (i = 31; i >= 0; i--)
	{
		t = v.l;
		s = p2[i];

		if (s > 31)
		{
			t = v.h;
			s -= 32;
		}
		r.h |= ((t >> s) & 1) << i;
	}
	return r;
}

/**
	\brief Расширяющая перестановка
	\param [inout] v блок 
*/
ulong64 ep(ulong v)
{
	uchar	p1[] = {
					31,  0,  1,  2,  3,  4,
					 3,  4,  5,  6,  7,  8,
					 7,  8,  9, 10, 11, 12,
					11, 12, 13, 14, 15, 16,
					15, 16, 17, 18, 19, 20,
					19, 20};
	uchar	p2[] = {
					21, 22, 23, 24,
					23, 24, 25, 26, 27, 28,
					27, 28, 29, 30, 31,  0};
	ulong64	r = {0, 0};
	int		i;

	// 32
	for (i = 31; i >= 0; i--)
	{
		r.l |= ((v >> p1[i]) & 1) << i;
	}
	// 16
	for (i = 15; i >= 0; i--)
	{
		r.h |= ((v >> p2[i]) & 1) << i;
	}
	return r;
}

/**
	\brief Матрица S
	\param [inout] v блок 
*/
ulong s_matrix(ulong64 v)
{
	static ulong	s1[] = {
					14u << 28u,  4u << 28u, 13u << 28u,  1u << 28u,  2u << 28u, 15u << 28u, 11u << 28u,  8u << 28u,  3u << 28u, 10u << 28u,  6u << 28u, 12u << 28u,  5u << 28u,  9u << 28u,  0u << 28u,  7u << 28u,
				 0u << 28u, 15u << 28u,  7u << 28u,  4u << 28u, 14u << 28u,  2u << 28u, 13u << 28u,  1u << 28u, 10u << 28u,  6u << 28u, 12u << 28u, 11u << 28u,  9u << 28u,  5u << 28u,  3u << 28u,  8u << 28u,
				 4u << 28u,  1u << 28u, 14u << 28u,  8u << 28u, 13u << 28u,  6u << 28u,  2u << 28u, 11u << 28u, 15u << 28u, 12u << 28u,  9u << 28u,  7u << 28u,  3u << 28u, 10u << 28u,  5u << 28u,  0u << 28u,
				15u << 28u, 12u << 28u,  8u << 28u,  2u << 28u,  4u << 28u,  9u << 28u,  1u << 28u,  7u << 28u,  5u << 28u, 11u << 28u,  3u << 28u, 14u << 28u, 10u << 28u,  0u << 28u,  6u << 28u, 13u << 28u
			};
	static ulong	s2[] = {
				15u << 24u,  1u << 24u,  8u << 24u, 14u << 24u,  6u << 24u, 11u << 24u,  3u << 24u,  4u << 24u,  9u << 24u,  7u << 24u,  2u << 24u, 13u << 24u, 12u << 24u,  0u << 24u,  5u << 24u, 10u << 24u,
				 3u << 24u, 13u << 24u,  4u << 24u,  7u << 24u, 15u << 24u,  2u << 24u,  8u << 24u, 14u << 24u, 12u << 24u,  0u << 24u,  1u << 24u, 10u << 24u,  6u<< 24u,  9u << 24u, 11u << 24u,  5u << 24u,
				 0u << 24u, 14u << 24u,  7u << 24u, 11u << 24u, 10u << 24u,  4u << 24u, 13u << 24u,  1u << 24u,  5u << 24u,  8u << 24u, 12u << 24u,  6u << 24u,  9u << 24u,  3u << 24u,  2u << 24u, 15u << 24u,
				13u << 24u,  8u << 24u, 10u << 24u,  1u << 24u,  3u << 24u, 15u << 24u,  4u << 24u,  2u << 24u, 11u << 24u,  6u << 24u,  7u << 24u, 12u << 24u,  0u << 24u,  5u<< 24u, 14u << 24u,  9u << 24u
			};
	static ulong	s3[] = {
				10u << 20u,  0u << 20u,  9u << 20u, 14u << 20u,  6u << 20u,  3u << 20u, 15u << 20u,  5u << 20u,  1u << 20u, 13u << 20u, 12u << 20u,  7u << 20u, 11u << 20u,  4u << 20u,  2u << 20u,  8u << 20u,
				13u << 20u,  7u << 20u,  0u << 20u,  9u << 20u,  3u << 20u,  4u << 20u,  6u << 20u, 10u << 20u,  2u << 20u,  8u << 20u,  5u << 20u, 14u << 20u, 12u << 20u, 11u << 20u, 15u << 20u,  1u << 20u,
				13u << 20u,  6u << 20u,  4u << 20u,  9u << 20u,  8u << 20u, 15u << 20u,  3u << 20u,  0u << 20u, 11u << 20u,  1u << 20u,  2u << 20u, 12u << 20u,  5u << 20u, 10u << 20u, 14u << 20u,  7u << 20u,
				 1u << 20u, 10u << 20u, 13u << 20u,  0u << 20u,  6u << 20u,  9u << 20u,  8u << 20u,  7u<< 20u,  4u << 20u, 15u << 20u, 14u << 20u,  3u << 20u, 11u << 20u,  5u << 20u,  2u << 20u, 12u << 20u
			};
	static ulong	s4[] = {
				 7 << 16, 13 << 16, 14 << 16,  3 << 16,  0 << 16,  6 << 16,  9 << 16, 10 << 16,  1 << 16,  2 << 16,  8 << 16,  5 << 16, 11 << 16, 12 << 16,  4 << 16, 15 << 16,
				13 << 16,  8 << 16, 11 << 16,  5 << 16,  6 << 16, 15 << 16,  0 << 16,  3 << 16,  4 << 16,  7 << 16,  2 << 16, 12 << 16,  1 << 16, 10 << 16, 14 << 16,  9 << 16,
				10 << 16,  6 << 16,  9 << 16,  0 << 16, 12 << 16, 11 << 16,  7 << 16, 13 << 16, 15 << 16,  1 << 16,  3 << 16, 14 << 16,  5 << 16,  2 << 16,  8 << 16,  4 << 16,
				 3 << 16, 15 << 16,  0 << 16,  6 << 16, 10 << 16,  1 << 16, 13 << 16,  8 << 16,  9 << 16,  4 << 16,  5 << 16, 11 << 16, 12 << 16,  7 << 16,  2 << 16, 14 << 16
			};
	static ulong	s5[] = {
				 2 << 12, 12 << 12,  4 << 12,  1 << 12,  7 << 12, 10 << 12, 11 << 12,  6 << 12,  8 << 12,  5 << 12,  3 << 12, 15 << 12, 13 << 12,  0 << 12, 14 << 12,  9 << 12,
				14 << 12, 11 << 12,  2 << 12, 12 << 12,  4 << 12,  7 << 12, 13 << 12,  1 << 12,  5 << 12,  0 << 12, 15 << 12, 10 << 12,  3 << 12,  9 << 12,  8 << 12,  6 << 12,
				 4 << 12,  2 << 12,  1 << 12, 11 << 12, 10 << 12, 13 << 12,  7 << 12,  8 << 12, 15 << 12,  9 << 12, 12 << 12,  5 << 12,  6 << 12,  3 << 12,  0 << 12, 14 << 12,
				11 << 12,  8 << 12, 12 << 12,  7 << 12,  1 << 12, 14 << 12,  2 << 12, 13 << 12,  6 << 12, 15 << 12,  0 << 12,  9 << 12, 10 << 12,  4 << 12,  5 << 12,  3 << 12
			};
	static ulong	s6[] = {
				12 << 8,  1 << 8, 10 << 8, 15 << 8,  9 << 8,  2 << 8,  6 << 8,  8 << 8,  0 << 8, 13 << 8,  3 << 8,  4 << 8, 14 << 8,  7 << 8,  5 << 8, 11 << 8,
				10 << 8, 15 << 8,  4 << 8,  2 << 8,  7 << 8, 12 << 8,  9 << 8,  5 << 8,  6 << 8,  1 << 8, 13 << 8, 14 << 8,  0 << 8, 11 << 8,  3 << 8,  8 << 8,
				 9 << 8, 14 << 8, 15 << 8,  5 << 8,  2 << 8,  8 << 8, 12 << 8,  3 << 8,  7 << 8,  0 << 8,  4 << 8, 10 << 8,  1 << 8, 13 << 8, 11 << 8,  6 << 8,
				 4 << 8,  3 << 8,  2 << 8, 12 << 8,  9 << 8,  5 << 8, 15 << 8, 10 << 8, 11 << 8, 14 << 8,  1 << 8,  7 << 8,  6 << 8,  0 << 8,  8 << 8, 13 << 8
			};
	static ulong	s7[] = {
				 4 << 4, 11 << 4,  2 << 4, 14 << 4, 15 << 4,  0 << 4,  8 << 4, 13 << 4,  3 << 4, 12 << 4,  9 << 4,  7 << 4,  5 << 4, 10 << 4,  6 << 4,  1 << 4,
				13 << 4,  0 << 4, 11 << 4,  7 << 4,  4 << 4,  9 << 4,  1 << 4, 10 << 4, 14 << 4,  3 << 4,  5 << 4, 12 << 4,  2 << 4, 15 << 4,  8 << 4,  6 << 4,
				 1 << 4,  4 << 4, 11 << 4, 13 << 4, 12 << 4,  3 << 4,  7 << 4, 14 << 4, 10 << 4, 15 << 4,  6 << 4,  8 << 4,  0 << 4,  5 << 4,  9 << 4,  2 << 4,
				 6 << 4, 11 << 4, 13 << 4,  8 << 4,  1 << 4,  4 << 4, 10 << 4,  7 << 4,  9 << 4,  5 << 4,  0 << 4, 15 << 4, 14 << 4,  2 << 4,  3 << 4, 12 << 4
			};
	static ulong	s8[] = {
				13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
				 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
				 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
				 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
			};

	v.h <<= 2;
	v.h |= (v.l >> 30);

	return
		s1[(((v.h >> 12) & 0x20) | ((v.h >>  8) & 0x10)) + ((v.h >> 13) & 0xF)] +
		s2[(((v.h >>  6) & 0x20) | ((v.h >>  2) & 0x10)) + ((v.h >>  7) & 0xF)] +
		s3[(((v.h      ) & 0x20) | ((v.h <<  4) & 0x10)) + ((v.h >>  1) & 0xF)] +
		s4[(((v.l >> 24) & 0x20) | ((v.l >> 20) & 0x10)) + ((v.l >> 25) & 0xF)] +
		s5[(((v.l >> 18) & 0x20) | ((v.l >> 14) & 0x10)) + ((v.l >> 19) & 0xF)] +
		s6[(((v.l >> 12) & 0x20) | ((v.l >>  8) & 0x10)) + ((v.l >> 13) & 0xF)] +
		s7[(((v.l >>  6) & 0x20) | ((v.l >>  2) & 0x10)) + ((v.l >>  7) & 0xF)] +
		s8[(((v.l      ) & 0x20) | ((v.l <<  4) & 0x10)) + ((v.l >>  1) & 0xF)];
}

/** 
	\brief Перестановка
	\param [inout] v блок 
*/
ulong p(ulong v)
{
	uchar	p1[] = {
					 7, 28, 21, 10, 26,  2, 19, 13,
					23, 29,  5,  0, 18,  8, 24, 30,
					22,  1, 14, 27,  6,  9, 17, 31,
					15,  4, 20,  3, 11, 12, 25, 16};
	ulong	r = 0;
	int		i;

	// 32
	for (i = 31; i >= 0; i--)
	{
		r |= ((v >> p1[i]) & 1) << i;
	}
	return r;
}

/**
	\brief Основное (раундовое) преобразование
	\param [inout] v блок
	\param [in] key ключ 
*/
ulong64 fk(ulong64 v, ulong64 key)
{
	ulong64	r = {0, 0};

	// расширяющая перестановка
	r = ep(v.l);

	// XOR с ключом
	r.h ^= key.h;
	r.l ^= key.l;

	// S матрица
	r.l = s_matrix(r);

	// просто перестановка
	r.l = p(r.l);

	// XOR с левой частью
	r.l ^= v.h;

	// перемещение правой части влево
	r.h = v.l;

	return r;
}

char xor_2_7_13_24(ulong v){
	return ((v & (1 << 2)) >> 2) ^ ((v & (1 << 7)) >> 7) ^ ((v & (1 << 13)) >> 13) ^ ((v & (1 << 24)) >> 24);
}

char fk_2_7_13_24(ulong v, ulong key)
{
	ulong64	r = {0, 0};

	// расширяющая перестановка
	r = ep(v);

	// XOR с ключом
	r.l ^= key;

	// S матрица
	r.l = s_matrix(r);

	// просто перестановка
	r.l = p(r.l);
	
	return xor_2_7_13_24(r.l);
}


/**
	\brief Основное (раундовое) преобразование
	\param [in] src пароль
	\param [out] key ключ 
*/
void des_create_keys(ulong64 src, ulong64 *key)
{
	uchar		ss[] = {1, 1, 2};
	uchar		s;
	ulong64		t;
	int			i;

	t = pc1(src);

	for (i = 0; i < 3; i++)
	{
		s = ss[i];

		t.l = (0x0FFFFFFF & (t.l << s)) | (t.l >> (28 - s));
		t.h = (0x0FFFFFFF & (t.h << s)) | (t.h >> (28 - s));

		key[i] = pc2(t);

	}

	key0 = key[0];
}	

/**
	Шифрование
	\param [inout] src входные данные
	\param [in] keys ключи 
*/
ulong64 des_encrypt_block(ulong64 src, ulong64 *keys)
{
	ulong64	r = {0, 0};
	ulong	t;

	// начальная перестановка
	r = ip(src);

	// 3 раунда
	r = fk(r, keys[0]);
	r = fk(r, keys[1]);
	r = fk(r, keys[2]);

	// свап
	t	= r.h;
	r.h	= r.l;
	r.l = t;

	// обратная к начальной перестановка
	r = rip(r);

	return r;
}


/**
	Дешифрование
	\param [inout] src входные данные
	\param [in] keys ключи 
*/
ulong64 des_decrypt_block(ulong64 src, ulong64 *keys)
{
	ulong64	r = {0, 0};
	ulong	t;

	// начальная перестановка
	r = ip(src);

	// 3 раунда
	r = fk(r, keys[2]);
	r = fk(r, keys[1]);
	r = fk(r, keys[0]);

	// свап
	t	= r.h;
	r.h	= r.l;
	r.l = t;

	// обратная к начальной перестановка
	r = rip(r);

	return r;
}

/**
	Шифрование по протоколу ecb
	\param [in] src входные данные
	\param [in] srclen длина входных данных
	\param [in] key ключ 
	\param [out] dst назначение 
*/
ulong des_encrypt_ecb(uchar *src, ulong srclen, uchar *dst, ulong64 key)
{
	uchar		buff[8];
	ulong64		keys[3];
	ulong64		r;
	uchar		*t = dst;

	if (srclen == 0)
		return 0;

	// prepare keys
	des_create_keys(key, keys);

	if (srclen >= 8)
	{
		do
		{
			// get 64 bits
			decode_rev(&r.h, src, 4);
			src += 4;
			decode_rev(&r.l, src, 4);
			src += 4;

			// encrypt
			r = des_encrypt_block(r, keys);

			// put 64 bits
			encode_rev(dst, &r.h, 4);
			dst += 4;
			encode_rev(dst, &r.l, 4);
			dst += 4;

			// 
			srclen -= 8;
		}
		while (srclen >= 8);
	}

	// crypt the rest
	if (srclen != 0)
	{
		memset(buff, 0, sizeof(buff));
		memcpy(buff, src, srclen);

		// get 64 bits
		decode_rev(&r.h, buff, 4);
		decode_rev(&r.l, buff + 4, 4);

		// encrypt
		r = des_encrypt_block(r, keys);

		// put 64 bits
		encode_rev(dst, &r.h, 4);
		dst += 4;
		encode_rev(dst, &r.l, 4);
		dst += 4;
	}

	return (dst - t);
}


/**
	Дешифрование по протоколу ecb
	\param [in] src входные данные
	\param [in] srclen длина входных данных
	\param [in] key ключ 
	\param [out] dst назначение 
*/
ulong des_decrypt_ecb(uchar *src, ulong srclen, uchar *dst, ulong64 key)
{
	ulong64		keys[16];
	ulong64		r;
	uchar		*t = dst;

	if (srclen < 8)
		return 0;

	// prepare keys
	des_create_keys(key, keys);

	do
	{
		// get 64 bits
		decode_rev(&r.h, src, 4);
		src += 4;
		decode_rev(&r.l, src, 4);
		src += 4;

		// encrypt
		r = des_decrypt_block(r, keys);

		// put 64 bits
		encode_rev(dst, &r.h, 4);
		dst += 4;
		encode_rev(dst, &r.l, 4);
		dst += 4;

		// 
		srclen -= 8;
	}
	while (srclen >= 8);

	return (dst - t);
}

/**
	Взлом des
	\param [in] src входные данные
	\param [in] srclen длина входных данных
	\param [out] dst назначение 
*/
ulong des_break(uchar *src, ulong srclen, uchar *dst)
{
	ulong64		pl;
	ulong64		ch;

	ulong T[64] = {0};
	ulong N = 0;

	do {
	// get 64 bits
		// cout << "Hello" << endl;
		decode_rev(&pl.h, src, 4);
		src += 4;
		decode_rev(&pl.l, src, 4);
		src += 4;
		// get 64 bits
		decode_rev(&ch.h, src, 4);
		src += 4;
		decode_rev(&ch.l, src, 4);
		src += 4;

		// N += 1;

		// cout << srclen << endl;

		for (int i = 0; i < 64; ++i)
		{
			if(xor_2_7_13_24(ch.h) ^ ((ch.l & (1 << 16)) >> 16) ^ xor_2_7_13_24(pl.h) ^ fk_2_7_13_24(pl.l, i << 24) == 0)
{
				T[i] += 1;
			}
		}

		srclen -= 8;
	} while(srclen >= 16);

	ulong res_max = 0;
	ulong res_i = 0;

	// cout << "Hello" << endl;

	for (int i = 0; i < 64; ++i)
	{
		cout << abs(long(T[i]) - long(N / 2)) << endl;
		if (abs(long(T[i]) - long(N / 2)) > res_max) 
{
			res_max = abs(long(T[i]) - long(N / 2));
			res_i = i; 
		}
	}

	return res_i;
}

map<string, int> ENGLISH_TRIGRAMS = {{ "ezo" , 1 }, { "apr" , 1 }, { "ref" , 2 }, { "nav" , 1 }, { "lyr" , 1 }, { "wle" , 1 }, { "pos" , 16 }, { "pto" , 4 }, { "tef" , 1 }, { "lla" , 3 }, { "hem" , 22 }, { "omr" , 1 }, { "sse" , 1 }, { "tth" , 9 }, { "dna" , 1 }, { "asu" , 3 }, { "rsd" , 1 }, { "opu" , 1 }, { "eds" , 1 }, { "tno" , 1 }, { "ndz" , 1 }, { "eha" , 1 }, { "ssa" , 22 }, { "ich" , 5 }, { "kss" , 1 }, { "tic" , 9 }, { "sth" , 23 }, { "fou" , 1 }, { "uli" , 1 }, { "ogr" , 3 }, { "osi" , 12 }, { "ker" , 3 }, { "aen" , 1 }, { "equ" , 5 }, { "kie" , 1 }, { "anh" , 1 }, { "igp" , 1 }, { "xto" , 4 }, { "uag" , 1 }, { "iae" , 1 }, { "yto" , 3 }, { "des" , 4 }, { "eol" , 1 }, { "iab" , 1 }, { "ewe" , 1 }, { "chi" , 6 }, { "phy" , 2 }, { "kth" , 2 }, { "ret" , 5 }, { "igu" , 1 }, { "rda" , 4 }, { "ndr" , 1 }, { "sso" , 3 }, { "tal" , 3 }, { "dha" , 1 }, { "mai" , 1 }, { "dva" , 2 }, { "bli" , 1 }, { "pes" , 3 }, { "ana" , 8 }, { "lvi" , 1 }, { "ust" , 4 }, { "liu" , 1 }, { "anu" , 1 }, { "rus" , 1 }, { "gci" , 1 }, { "hni" , 2 }, { "sen" , 5 }, { "ema" , 5 }, { "gfo" , 1 }, { "obr" , 2 }, { "fth" , 20 }, { "ngl" , 3 }, { "cry" , 9 }, { "etb" , 2 }, { "ran" , 23 }, { "raj" , 1 }, { "mit" , 1 }, { "sof" , 16 }, { "ame" , 8 }, { "ckc" , 1 }, { "had" , 1 }, { "hab" , 28 }, { "kwa" , 1 }, { "wha" , 5 }, { "git" , 1 }, { "otb" , 1 }, { "fte" , 4 }, { "hya" , 1 }, { "net" , 3 }, { "bys" , 1 }, { "ehy" , 1 }, { "sat" , 3 }, { "kso" , 1 }, { "oni" , 1 }, { "esh" , 2 }, { "mov" , 1 }, { "ubl" , 1 }, { "sch" , 5 }, { "dsa" , 1 }, { "ewc" , 1 }, { "ebr" , 3 }, { "asj" , 1 }, { "gou" , 1 }, { "blo" , 1 }, { "yta" , 1 }, { "kep" , 1 }, { "sar" , 22 }, { "pro" , 3 }, { "byf" , 1 }, { "col" , 7 }, { "sma" , 4 }, { "rsf" , 1 }, { "beo" , 1 }, { "ldl" , 2 }, { "ewr" , 1 }, { "ksb" , 1 }, { "hec" , 18 }, { "cit" , 2 }, { "edi" , 8 }, { "rse" , 3 }, { "lum" , 7 }, { "ryi" , 1 }, { "hoc" , 1 }, { "hou" , 4 }, { "bef" , 1 }, { "dto" , 11 }, { "fle" , 4 }, { "orw" , 2 }, { "lss" , 1 }, { "its" , 3 }, { "fmo" , 1 }, { "ena" , 1 }, { "rma" , 2 }, { "now" , 7 }, { "iga" , 1 }, { "isb" , 8 }, { "rpa" , 2 }, { "hha" , 1 }, { "per" , 2 }, { "mac" , 3 }, { "tis" , 11 }, { "nan" , 6 }, { "gat" , 1 }, { "ned" , 3 }, { "ndo" , 4 }, { "esf" , 1 }, { "fet" , 1 }, { "sty" , 1 }, { "agr" , 1 }, { "lec" , 3 }, { "hto" , 1 }, { "nge" , 9 }, { "veu" , 1 }, { "cur" , 1 }, { "tim" , 3 }, { "swe" , 4 }, { "sil" , 2 }, { "owb" , 1 }, { "rdo" , 3 }, { "ign" , 4 }, { "nak" , 2 }, { "fnu" , 1 }, { "cat" , 2 }, { "ngf" , 1 }, { "reu" , 6 }, { "tco" , 1 }, { "and" , 36 }, { "ert" , 24 }, { "ong" , 4 }, { "nov" , 2 }, { "ofr" , 3 }, { "iou" , 3 }, { "dit" , 1 }, { "uct" , 1 }, { "nbe" , 3 }, { "pec" , 3 }, { "ype" , 4 }, { "cor" , 6 }, { "rac" , 2 }, { "nam" , 2 }, { "onw" , 2 }, { "mle" , 1 }, { "lat" , 3 }, { "fir" , 6 }, { "tst" , 2 }, { "ckt" , 1 }, { "fro" , 4 }, { "lnl" , 1 }, { "tab" , 1 }, { "bym" , 1 }, { "ess" , 25 }, { "ubs" , 17 }, { "ekn" , 1 }, { "ltt" , 1 }, { "yte" , 1 }, { "szo" , 1 }, { "ott" , 1 }, { "esc" , 1 }, { "fic" , 2 }, { "inc" , 4 }, { "xin" , 1 }, { "rme" , 6 }, { "fsu" , 2 }, { "ctp" , 1 }, { "ecu" , 1 }, { "cla" , 15 }, { "cco" , 2 }, { "gsu" , 1 }, { "lsh" , 1 }, { "dsf" , 1 }, { "bre" , 3 }, { "ngk" , 1 }, { "ude" , 2 }, { "nsu" , 2 }, { "erc" , 3 }, { "edp" , 1 }, { "epi" , 1 }, { "smu" , 1 }, { "dse" , 1 }, { "pon" , 4 }, { "isw" , 2 }, { "ssu" , 10 }, { "lfu" , 1 }, { "meh" , 1 }, { "arr" , 5 }, { "leh" , 2 }, { "sby" , 2 }, { "icw" , 1 }, { "ldh" , 1 }, { "twh" , 1 }, { "tur" , 1 }, { "bro" , 2 }, { "ede" , 2 }, { "rer" , 2 }, { "cre" , 3 }, { "yst" , 4 }, { "amo" , 2 }, { "lis" , 2 }, { "eon" , 4 }, { "nto" , 4 }, { "nli" , 1 }, { "byw" , 1 }, { "yis" , 4 }, { "rsh" , 4 }, { "stm" , 1 }, { "aju" , 2 }, { "whi" , 5 }, { "nbi" , 1 }, { "lue" , 2 }, { "ate" , 10 }, { "rid" , 1 }, { "bes" , 1 }, { "sti" , 18 }, { "iff" , 4 }, { "tia" , 1 }, { "uwa" , 1 }, { "pco" , 1 }, { "tav" , 1 }, { "ath" , 2 }, { "nex" , 4 }, { "own" , 4 }, { "ped" , 5 }, { "ewo" , 2 }, { "cae" , 6 }, { "las" , 16 }, { "dke" , 1 }, { "atu" , 2 }, { "yar" , 2 }, { "xtf" , 1 }, { "nsn" , 1 }, { "ork" , 1 }, { "tea" , 3 }, { "ilo" , 1 }, { "hci" , 4 }, { "onl" , 5 }, { "dby" , 9 }, { "ata" , 4 }, { "lew" , 1 }, { "nsf" , 1 }, { "ita" , 4 }, { "nco" , 7 }, { "omm" , 1 }, { "nac" , 1 }, { "fto" , 1 }, { "twi" , 1 }, { "mod" , 4 }, { "saw" , 1 }, { "ptt" , 1 }, { "ymi" , 1 }, { "rly" , 1 }, { "tyo" , 4 }, { "eti" , 14 }, { "ski" , 1 }, { "cdo" , 1 }, { "owi" , 1 }, { "nnu" , 1 }, { "lei" , 6 }, { "ves" , 3 }, { "afi" , 1 }, { "ewt" , 1 }, { "csu" , 3 }, { "llk" , 3 }, { "sfy" , 1 }, { "cwa" , 1 }, { "oun" , 1 }, { "tos" , 3 }, { "phr" , 1 }, { "tbo" , 1 }, { "uen" , 5 }, { "ldu" , 1 }, { "ugh" , 2 }, { "rea" , 18 }, { "rst" , 11 }, { "heo" , 5 }, { "ual" , 1 }, { "alc" , 16 }, { "fhe" , 2 }, { "gor" , 2 }, { "chn" , 4 }, { "yat" , 3 }, { "eco" , 14 }, { "ult" , 4 }, { "abe" , 27 }, { "ose" , 9 }, { "sre" , 4 }, { "sin" , 23 }, { "eyi" , 2 }, { "lom" , 1 }, { "tem" , 4 }, { "ldt" , 1 }, { "oos" , 2 }, { "lpa" , 1 }, { "tob" , 4 }, { "ckw" , 1 }, { "gma" , 1 }, { "wsa" , 1 }, { "ass" , 16 }, { "dew" , 1 }, { "ees" , 1 }, { "nls" , 1 }, { "cha" , 11 }, { "cle" , 3 }, { "ila" , 2 }, { "dda" , 2 }, { "rew" , 1 }, { "rev" , 3 }, { "olo" , 3 }, { "rto" , 6 }, { "dig" , 1 }, { "ang" , 9 }, { "cth" , 1 }, { "tsy" , 1 }, { "lly" , 6 }, { "gen" , 7 }, { "nev" , 1 }, { "myn" , 1 }, { "aus" , 3 }, { "xtl" , 1 }, { "cya" , 3 }, { "lar" , 3 }, { "und" , 4 }, { "cde" , 1 }, { "nks" , 1 }, { "ble" , 11 }, { "asy" , 3 }, { "ncy" , 4 }, { "ecb" , 1 }, { "niq" , 2 }, { "com" , 4 }, { "lve" , 4 }, { "rnb" , 1 }, { "wri" , 3 }, { "cho" , 5 }, { "mul" , 2 }, { "hgl" , 1 }, { "req" , 5 }, { "rne" , 2 }, { "rtr" , 2 }, { "cks" , 9 }, { "gua" , 1 }, { "sex" , 1 }, { "dno" , 2 }, { "age" , 25 }, { "odi" , 4 }, { "ide" , 4 }, { "nbl" , 1 }, { "tta" , 11 }, { "eme" , 19 }, { "efo" , 5 }, { "fme" , 2 }, { "dou" , 1 }, { "ryr" , 1 }, { "sop" , 2 }, { "nif" , 3 }, { "erd" , 1 }, { "ntl" , 3 }, { "ngn" , 1 }, { "ina" , 10 }, { "toc" , 2 }, { "gpe" , 2 }, { "vin" , 2 }, { "kes" , 1 }, { "les" , 9 }, { "dot" , 4 }, { "stf" , 1 }, { "tee" , 2 }, { "ghs" , 1 }, { "tbe" , 3 }, { "oas" , 1 }, { "dcf" , 1 }, { "hcl" , 1 }, { "aft" , 1 }, { "rdi" , 7 }, { "eal" , 11 }, { "cal" , 23 }, { "ngr" , 1 }, { "acw" , 1 }, { "mew" , 1 }, { "tac" , 11 }, { "orc" , 4 }, { "min" , 4 }, { "ftt" , 1 }, { "mwi" , 1 }, { "cht" , 2 }, { "ifh" , 2 }, { "nec" , 3 }, { "rap" , 2 }, { "ehe" , 1 }, { "ntt" , 2 }, { "ood" , 4 }, { "lia" , 1 }, { "edt" , 13 }, { "eim" , 2 }, { "ems" , 1 }, { "ncl" , 4 }, { "lsp" , 1 }, { "eea" , 2 }, { "ane" , 1 }, { "tsp" , 1 }, { "efi" , 5 }, { "ove" , 4 }, { "ydb" , 1 }, { "nyl" , 1 }, { "lem" , 2 }, { "ngo" , 2 }, { "war" , 1 }, { "yaf" , 1 }, { "cty" , 1 }, { "dwa" , 1 }, { "ona" , 6 }, { "exa" , 19 }, { "rsa" , 14 }, { "tra" , 15 }, { "awo" , 1 }, { "snt" , 1 }, { "ean" , 3 }, { "ook" , 3 }, { "ngp" , 1 }, { "you" , 13 }, { "emo" , 5 }, { "nee" , 3 }, { "ffe" , 3 }, { "tat" , 6 }, { "ebu" , 1 }, { "peo" , 3 }, { "aga" , 2 }, { "aci" , 3 }, { "woo" , 1 }, { "iea" , 1 }, { "qua" , 4 }, { "eir" , 5 }, { "tro" , 5 }, { "rit" , 5 }, { "med" , 2 }, { "ale" , 2 }, { "sem" , 1 }, { "eyo" , 6 }, { "gey" , 5 }, { "chs" , 2 }, { "nig" , 1 }, { "bst" , 17 }, { "pop" , 1 }, { "chc" , 4 }, { "tof" , 9 }, { "eka" , 1 }, { "yus" , 2 }, { "eas" , 4 }, { "ave" , 5 }, { "hoo" , 2 }, { "ump" , 1 }, { "wel" , 4 }, { "ptw" , 1 }, { "xth" , 1 }, { "nap" , 2 }, { "odw" , 1 }, { "utb" , 1 }, { "him" , 1 }, { "zod" , 2 }, { "bet" , 29 }, { "ctc" , 1 }, { "heg" , 1 }, { "byh" , 1 }, { "rej" , 1 }, { "tet" , 2 }, { "dtr" , 3 }, { "oll" , 1 }, { "sed" , 17 }, { "gse" , 1 }, { "ain" , 8 }, { "yll" , 2 }, { "ent" , 16 }, { "ali" , 3 }, { "kst" , 1 }, { "stc" , 1 }, { "ntc" , 2 }, { "ode" , 17 }, { "ely" , 1 }, { "urm" , 2 }, { "rpr" , 1 }, { "one" , 5 }, { "gna" , 2 }, { "ime" , 3 }, { "nst" , 8 }, { "oor" , 1 }, { "mna" , 1 }, { "lac" , 7 }, { "eye" , 1 }, { "way" , 1 }, { "hey" , 2 }, { "hek" , 8 }, { "lya" , 8 }, { "wso" , 1 }, { "etw" , 4 }, { "dpe" , 1 }, { "wnc" , 1 }, { "era" , 25 }, { "sim" , 5 }, { "wit" , 12 }, { "rep" , 13 }, { "etc" , 3 }, { "tai" , 1 }, { "enp" , 2 }, { "igi" , 4 }, { "aph" , 2 }, { "ven" , 5 }, { "tus" , 2 }, { "ppl" , 1 }, { "dza" , 1 }, { "ary" , 1 }, { "gnu" , 1 }, { "mil" , 2 }, { "sha" , 2 }, { "uwo" , 4 }, { "eli" , 1 }, { "rwi" , 3 }, { "isa" , 9 }, { "bru" , 1 }, { "tcw" , 1 }, { "etm" , 1 }, { "nsa" , 1 }, { "sis" , 11 }, { "tit" , 19 }, { "itt" , 1 }, { "utu" , 1 }, { "cke" , 3 }, { "bot" , 2 }, { "sag" , 24 }, { "tpa" , 1 }, { "nca" , 1 }, { "rya" , 1 }, { "iqu" , 2 }, { "nun" , 4 }, { "oso" , 2 }, { "uet" , 1 }, { "vat" , 1 }, { "nta" , 2 }, { "tsa" , 7 }, { "ets" , 4 }, { "sic" , 16 }, { "ery" , 3 }, { "asa" , 2 }, { "gea" , 3 }, { "mei" , 1 }, { "don" , 6 }, { "ost" , 2 }, { "rex" , 11 }, { "int" , 22 }, { "asb" , 1 }, { "erp" , 3 }, { "epo" , 1 }, { "dan" , 2 }, { "rut" , 1 }, { "raf" , 1 }, { "for" , 26 }, { "eci" , 21 }, { "alg" , 1 }, { "tve" , 1 }, { "enc" , 24 }, { "rnc" , 2 }, { "ksi" , 2 }, { "iaj" , 1 }, { "cil" , 1 }, { "urc" , 1 }, { "rbe" , 2 }, { "usc" , 4 }, { "ntd" , 1 }, { "nas" , 3 }, { "wis" , 1 }, { "lea" , 3 }, { "wor" , 15 }, { "phe" , 83 }, { "ols" , 5 }, { "lyt" , 2 }, { "erw" , 8 }, { "cou" , 2 }, { "tsh" , 1 }, { "lld" , 1 }, { "sho" , 4 }, { "ors" , 4 }, { "ive" , 2 }, { "otv" , 1 }, { "gek" , 1 }, { "eto" , 11 }, { "esi" , 7 }, { "ete" , 2 }, { "our" , 4 }, { "vem" , 1 }, { "upi" , 1 }, { "omy" , 1 }, { "nfa" , 1 }, { "esu" , 10 }, { "see" , 2 }, { "orr" , 4 }, { "nsh" , 1 }, { "tpl" , 1 }, { "typ" , 4 }, { "scl" , 2 }, { "arb" , 1 }, { "rct" , 1 }, { "ots" , 5 }, { "ids" , 2 }, { "utr" , 1 }, { "ron" , 3 }, { "aki" , 1 }, { "bac" , 1 }, { "sst" , 1 }, { "leb" , 1 }, { "ica" , 23 }, { "dar" , 2 }, { "gmo" , 2 }, { "mpt" , 1 }, { "bol" , 4 }, { "wbe" , 1 }, { "edf" , 3 }, { "qui" , 1 }, { "hst" , 3 }, { "rwo" , 5 }, { "geo" , 3 }, { "rco" , 2 }, { "lee" , 1 }, { "ofn" , 1 }, { "ser" , 1 }, { "edg" , 1 }, { "onc" , 23 }, { "sot" , 2 }, { "pai" , 2 }, { "dos" , 1 }, { "rks" , 1 }, { "lyx" , 1 }, { "cea" , 5 }, { "vel" , 2 }, { "een" , 8 }, { "gkn" , 1 }, { "ssc" , 1 }, { "ele" , 17 }, { "utt" , 2 }, { "rdb" , 1 }, { "scr" , 3 }, { "inv" , 4 }, { "dec" , 3 }, { "ley" , 1 }, { "squ" , 4 }, { "art" , 7 }, { "wyo" , 2 }, { "oaf" , 1 }, { "tak" , 5 }, { "nce" , 9 }, { "osc" , 1 }, { "swh" , 5 }, { "tut" , 17 }, { "who" , 1 }, { "ish" , 4 }, { "ont" , 3 }, { "her" , 104 }, { "gme" , 1 }, { "ues" , 2 }, { "alm" , 1 }, { "ehk" , 1 }, { "oan" , 1 }, { "epe" , 4 }, { "son" , 2 }, { "ice" , 2 }, { "ofs" , 2 }, { "hev" , 5 }, { "sun" , 1 }, { "sut" , 1 }, { "umb" , 8 }, { "lor" , 2 }, { "ngt" , 10 }, { "ink" , 1 }, { "sit" , 12 }, { "htw" , 1 }, { "alo" , 2 }, { "tou" , 1 }, { "oub" , 1 }, { "uti" , 17 }, { "sou" , 1 }, { "ety" , 2 }, { "ene" , 10 }, { "ear" , 9 }, { "noa" , 1 }, { "epr" , 5 }, { "ace" , 9 }, { "ddo" , 2 }, { "ver" , 7 }, { "tme" , 1 }, { "mix" , 1 }, { "mnu" , 3 }, { "sys" , 4 }, { "gns" , 1 }, { "aba" , 1 }, { "lai" , 5 }, { "ash" , 3 }, { "cip" , 83 }, { "htt" , 1 }, { "cby" , 1 }, { "hav" , 6 }, { "yan" , 6 }, { "dsc" , 1 }, { "nth" , 16 }, { "iec" , 1 }, { "yac" , 2 }, { "icd" , 1 }, { "itu" , 18 }, { "yfe" , 1 }, { "rsy" , 1 }, { "tex" , 15 }, { "rdt" , 3 }, { "epl" , 6 }, { "bit" , 1 }, { "dcr" , 1 }, { "kyi" , 2 }, { "sub" , 17 }, { "cul" , 1 }, { "ndu" , 2 }, { "tip" , 2 }, { "ykn" , 1 }, { "vid" , 1 }, { "ans" , 18 }, { "bas" , 3 }, { "rso" , 11 }, { "app" , 1 }, { "two" , 4 }, { "cod" , 11 }, { "ket" , 5 }, { "llr" , 1 }, { "eof" , 11 }, { "zln" , 1 }, { "nde" , 5 }, { "thm" , 1 }, { "enu" , 3 }, { "hsy" , 2 }, { "scy" , 1 }, { "sul" , 1 }, { "fpo" , 1 }, { "has" , 10 }, { "ami" , 1 }, { "ade" , 1 }, { "dle" , 1 }, { "lci" , 15 }, { "inu" , 1 }, { "key" , 15 }, { "rba" , 2 }, { "sev" , 2 }, { "nte" , 8 }, { "ula" , 1 }, { "por" , 1 }, { "dof" , 6 }, { "hes" , 17 }, { "efe" , 2 }, { "olu" , 7 }, { "iac" , 2 }, { "cra" , 3 }, { "hen" , 14 }, { "nvo" , 4 }, { "sor" , 8 }, { "hco" , 1 }, { "ods" , 2 }, { "nes" , 5 }, { "eys" , 3 }, { "nwi" , 2 }, { "lgo" , 1 }, { "emb" , 1 }, { "spe" , 3 }, { "enr" , 1 }, { "ord" , 23 }, { "icl" , 2 }, { "lds" , 1 }, { "dey" , 1 }, { "ror" , 3 }, { "mou" , 1 }, { "cet" , 2 }, { "aco" , 1 }, { "ing" , 28 }, { "ten" , 6 }, { "pli" , 1 }, { "aty" , 2 }, { "fyt" , 1 }, { "ern" , 9 }, { "err" , 1 }, { "ouf" , 1 }, { "ore" , 21 }, { "urd" , 1 }, { "ns" , 1 }, { "tio" , 33 }, { "ght" , 3 }, { "asm" , 1 }, { "cyt" , 1 }, { "ibl" , 4 }, { "all" , 13 }, { "upe" , 1 }, { "rch" , 1 }, { "dju" , 1 }, { "ine" , 12 }, { "iti" , 13 }, { "yss" , 1 }, { "isr" , 2 }, { "she" , 5 }, { "lan" , 2 }, { "the" , 134 }, { "syt" , 2 }, { "ral" , 19 }, { "mad" , 1 }, { "ywe" , 1 }, { "wnp" , 1 }, { "irm" , 1 }, { "sef" , 2 }, { "nea" , 3 }, { "ryw" , 1 }, { "eom" , 1 }, { "tcf" , 2 }, { "lyq" , 1 }, { "ngc" , 1 }, { "lab" , 2 }, { "sfa" , 1 }, { "ard" , 2 }, { "rin" , 2 }, { "ryl" , 1 }, { "yth" , 7 }, { "eup" , 1 }, { "amb" , 2 }, { "hsi" , 1 }, { "rmo" , 4 }, { "fig" , 1 }, { "usu" , 1 }, { "ero" , 7 }, { "ago" , 1 }, { "yin" , 3 }, { "rri" , 1 }, { "lex" , 2 }, { "ofp" , 2 }, { "gew" , 3 }, { "wan" , 1 }, { "new" , 2 }, { "lme" , 1 }, { "hef" , 5 }, { "mes" , 29 }, { "set" , 6 }, { "not" , 7 }, { "ted" , 8 }, { "olv" , 4 }, { "vet" , 2 }, { "ais" , 2 }, { "ndt" , 12 }, { "sce" , 3 }, { "eus" , 10 }, { "ewh" , 4 }, { "gld" , 1 }, { "jup" , 1 }, { "toa" , 4 }, { "eyw" , 9 }, { "cta" , 2 }, { "gur" , 1 }, { "bya" , 1 }, { "aes" , 6 }, { "sib" , 1 }, { "nus" , 1 }, { "lcl" , 1 }, { "rsn" , 1 }, { "ano" , 4 }, { "inn" , 2 }, { "ybe" , 1 }, { "rek" , 1 }, { "mam" , 1 }, { "ffi" , 2 }, { "rcr" , 1 }, { "tdi" , 1 }, { "shi" , 2 }, { "asl" , 1 }, { "lsc" , 2 }, { "ngu" , 1 }, { "rel" , 5 }, { "msc" , 1 }, { "stb" , 2 }, { "edb" , 9 }, { "ndb" , 1 }, { "ynu" , 1 }, { "pig" , 1 }, { "lov" , 1 }, { "rge" , 1 }, { "dal" , 2 }, { "rsw" , 5 }, { "sht" , 1 }, { "nsp" , 16 }, { "urn" , 1 }, { "sbe" , 3 }, { "usi" , 5 }, { "dhe" , 2 }, { "ivi" , 1 }, { "dsi" , 2 }, { "mmo" , 1 }, { "xya" , 1 }, { "tot" , 4 }, { "bfo" , 1 }, { "ldb" , 3 }, { "rra" , 4 }, { "akm" , 1 }, { "hea" , 6 }, { "stp" , 1 }, { "nal" , 11 }, { "duc" , 2 }, { "din" , 10 }, { "ext" , 18 }, { "rnu" , 1 }, { "orp" , 1 }, { "rsu" , 1 }, { "til" , 2 }, { "abl" , 6 }, { "ger" , 3 }, { "ces" , 5 }, { "ach" , 8 }, { "ram" , 2 }, { "dfo" , 6 }, { "soa" , 1 }, { "esm" , 1 }, { "nse" , 1 }, { "ndi" , 3 }, { "obo" , 1 }, { "pti" , 5 }, { "ngm" , 4 }, { "tin" , 9 }, { "ssy" , 1 }, { "bea" , 2 }, { "eab" , 1 }, { "ycl" , 2 }, { "sig" , 4 }, { "wna" , 1 }, { "nfo" , 1 }, { "suc" , 7 }, { "erf" , 1 }, { "cau" , 3 }, { "eev" , 1 }, { "gle" , 3 }, { "mel" , 1 }, { "owr" , 1 }, { "ssi" , 17 }, { "sta" , 7 }, { "aid" , 1 }, { "oen" , 5 }, { "asf" , 2 }, { "fin" , 4 }, { "ock" , 1 }, { "rfr" , 1 }, { "gec" , 1 }, { "sid" , 1 }, { "dop" , 1 }, { "awi" , 1 }, { "nsi" , 2 }, { "har" , 1 }, { "beb" , 2 }, { "gwh" , 2 }, { "odo" , 2 }, { "ctr" , 1 }, { "get" , 3 }, { "eso" , 7 }, { "wss" , 1 }, { "tss" , 1 }, { "evi" , 5 }, { "att" , 11 }, { "udi" , 1 }, { "egt" , 1 }, { "shs" , 1 }, { "sew" , 1 }, { "oex" , 1 }, { "hod" , 5 }, { "lph" , 27 }, { "omp" , 2 }, { "uwi" , 1 }, { "iso" , 2 }, { "gam" , 1 }, { "rna" , 2 }, { "ift" , 5 }, { "atc" , 3 }, { "ptu" , 1 }, { "hez" , 1 }, { "cia" , 1 }, { "nly" , 5 }, { "ses" , 5 }, { "ase" , 4 }, { "red" , 5 }, { "gth" , 7 }, { "tec" , 4 }, { "bab" , 1 }, { "spl" , 1 }, { "als" , 7 }, { "any" , 8 }, { "tor" , 7 }, { "eor" , 4 }, { "ofi" , 3 }, { "lyu" , 1 }, { "rta" , 1 }, { "ill" , 1 }, { "oci" , 1 }, { "nit" , 1 }, { "upc" , 1 }, { "ply" , 1 }, { "esd" , 1 }, { "sus" , 10 }, { "eyb" , 1 }, { "ons" , 7 }, { "akt" , 1 }, { "san" , 15 }, { "cyc" , 1 }, { "ixi" , 1 }, { "hyt" , 1 }, { "ndc" , 4 }, { "ipl" , 2 }, { "ded" , 3 }, { "rot" , 4 }, { "sbu" , 2 }, { "gef" , 1 }, { "tba" , 1 }, { "kas" , 1 }, { "yso" , 1 }, { "lst" , 1 }, { "ori" , 6 }, { "adt" , 1 }, { "byd" , 1 }, { "yec" , 1 }, { "sbf" , 1 }, { "tto" , 5 }, { "asc" , 2 }, { "ypt" , 9 }, { "gho" , 1 }, { "byi" , 1 }, { "nda" , 1 }, { "rod" , 2 }, { "lre" , 1 }, { "led" , 2 }, { "swi" , 1 }, { "aas" , 1 }, { "sei" , 3 }, { "pso" , 2 }, { "owe" , 1 }, { "oea" , 1 }, { "rwh" , 2 }, { "dmo" , 3 }, { "isz" , 1 }, { "edk" , 1 }, { "asi" , 4 }, { "rli" , 1 }, { "loo" , 3 }, { "gai" , 2 }, { "rem" , 4 }, { "aca" , 1 }, { "thr" , 4 }, { "mbe" , 9 }, { "nno" , 1 }, { "ldd" , 1 }, { "mor" , 8 }, { "iaa" , 1 }, { "tas" , 2 }, { "rom" , 5 }, { "eee" , 1 }, { "cin" , 1 }, { "das" , 3 }, { "ial" , 2 }, { "tow" , 2 }, { "ama" , 2 }, { "isd" , 1 }, { "twa" , 1 }, { "kma" , 1 }, { "gya" , 1 }, { "bee" , 2 }, { "eex" , 2 }, { "rph" , 1 }, { "ban" , 2 }, { "oup" , 3 }, { "ndw" , 1 }, { "ion" , 33 }, { "ups" , 2 }, { "tog" , 3 }, { "iss" , 2 }, { "ims" , 1 }, { "gre" , 2 }, { "fal" , 1 }, { "dca" , 2 }, { "uci" , 1 }, { "ved" , 1 }, { "sla" , 1 }, { "sbl" , 2 }, { "ayt" , 1 }, { "nre" , 1 }, { "tri" , 1 }, { "anb" , 3 }, { "uts" , 1 }, { "som" , 6 }, { "far" , 1 }, { "ect" , 6 }, { "hms" , 1 }, { "rat" , 4 }, { "rsc" , 6 }, { "ome" , 9 }, { "fpl" , 1 }, { "clu" , 3 }, { "ant" , 1 }, { "mal" , 1 }, { "tol" , 1 }, { "ofo" , 1 }, { "tci" , 5 }, { "ofc" , 7 }, { "orm" , 3 }, { "esp" , 7 }, { "rok" , 2 }, { "eac" , 6 }, { "ers" , 68 }, { "rmi" , 2 }, { "ute" , 3 }, { "bye" , 1 }, { "laa" , 1 }, { "ldc" , 1 }, { "sfo" , 7 }, { "eep" , 1 }, { "kip" , 2 }, { "lls" , 1 }, { "ins" , 4 }, { "ili" , 2 }, { "iki" , 2 }, { "pta" , 4 }, { "ksl" , 1 }, { "mni" , 1 }, { "ise" , 2 }, { "rou" , 5 }, { "rei" , 2 }, { "unc" , 2 }, { "mea" , 1 }, { "ein" , 5 }, { "wev" , 1 }, { "bec" , 5 }, { "bby" , 1 }, { "dmu" , 1 }, { "uld" , 16 }, { "org" , 2 }, { "upt" , 1 }, { "eat" , 9 }, { "sno" , 2 }, { "dso" , 1 }, { "top" , 1 }, { "stl" , 3 }, { "tar" , 5 }, { "rec" , 7 }, { "hee" , 4 }, { "num" , 9 }, { "loc" , 1 }, { "sua" , 1 }, { "aly" , 5 }, { "erl" , 3 }, { "ldj" , 1 }, { "zan" , 1 }, { "ndh" , 2 }, { "ftr" , 1 }, { "ndn" , 1 }, { "oft" , 20 }, { "jul" , 1 }, { "taw" , 1 }, { "eis" , 9 }, { "oma" , 1 }, { "cri" , 1 }, { "okl" , 2 }, { "dis" , 3 }, { "sba" , 1 }, { "yle" , 2 }, { "use" , 26 }, { "owt" , 1 }, { "ict" , 1 }, { "cep" , 3 }, { "yqu" , 1 }, { "ras" , 1 }, { "rcd" , 1 }, { "ith" , 13 }, { "etr" , 3 }, { "cte" , 1 }, { "ndm" , 2 }, { "edh" , 1 }, { "npo" , 1 }, { "omn" , 1 }, { "man" , 7 }, { "usa" , 1 }, { "oag" , 1 }, { "but" , 6 }, { "bly" , 1 }, { "oal" , 1 }, { "cti" , 1 }, { "dge" , 1 }, { "eap" , 1 }, { "sie" , 1 }, { "nya" , 1 }, { "elo" , 2 }, { "row" , 3 }, { "eng" , 1 }, { "szl" , 1 }, { "obe" , 3 }, { "tie" , 1 }, { "ybu" , 1 }, { "adv" , 2 }, { "eta" , 1 }, { "tse" , 1 }, { "han" , 7 }, { "ist" , 9 }, { "gro" , 3 }, { "opt" , 2 }, { "byu" , 1 }, { "ngs" , 2 }, { "est" , 9 }, { "lef" , 5 }, { "hid" , 1 }, { "sme" , 1 }, { "ado" , 2 }, { "sel" , 4 }, { "hel" , 12 }, { "apo" , 3 }, { "ton" , 5 }, { "dma" , 1 }, { "dog" , 1 }, { "icc" , 1 }, { "rsb" , 1 }, { "rob" , 1 }, { "tam" , 2 }, { "wou" , 13 }, { "gto" , 4 }, { "vig" , 5 }, { "lys" , 6 }, { "gne" , 1 }, { "pea" , 4 }, { "ntn" , 1 }, { "rte" , 10 }, { "oss" , 1 }, { "sgr" , 1 }, { "aan" , 2 }, { "nyc" , 2 }, { "abr" , 1 }, { "mer" , 1 }, { "acc" , 2 }, { "can" , 6 }, { "esg" , 1 }, { "dsw" , 1 }, { "dow" , 1 }, { "otr" , 2 }, { "erh" , 2 }, { "asz" , 1 }, { "ken" , 4 }, { "eru" , 1 }, { "mne" , 1 }, { "jus" , 3 }, { "dor" , 3 }, { "nts" , 2 }, { "uit" , 1 }, { "hra" , 1 }, { "erm" , 9 }, { "ars" , 1 }, { "pot" , 1 }, { "nar" , 2 }, { "chl" , 2 }, { "pts" , 1 }, { "isu" , 3 }, { "gei" , 4 }, { "goo" , 1 }, { "isc" , 4 }, { "isf" , 1 }, { "edu" , 1 }, { "eft" , 2 }, { "uns" , 1 }, { "pla" , 11 }, { "ree" , 3 }, { "nch" , 1 }, { "nol" , 3 }, { "yev" , 1 }, { "utk" , 1 }, { "ber" , 9 }, { "iph" , 83 }, { "vec" , 1 }, { "owl" , 1 }, { "cfi" , 1 }, { "rio" , 2 }, { "dhi" , 1 }, { "sam" , 5 }, { "dbu" , 1 }, { "mpu" , 1 }, { "mpr" , 1 }, { "ope" , 4 }, { "sfr" , 1 }, { "tla" , 2 }, { "ify" , 1 }, { "gyw" , 1 }, { "spa" , 3 }, { "icu" , 1 }, { "too" , 4 }, { "ksu" , 1 }, { "keu" , 1 }, { "mus" , 1 }, { "rci" , 5 }, { "orb" , 2 }, { "dwi" , 3 }, { "enf" , 2 }, { "cci" , 1 }, { "rns" , 2 }, { "ouw" , 6 }, { "hno" , 2 }, { "raa" , 1 }, { "gri" , 1 }, { "clo" , 1 }, { "ngw" , 2 }, { "nds" , 4 }, { "rde" , 6 }, { "eed" , 1 }, { "sca" , 7 }, { "ino" , 2 }, { "ile" , 1 }, { "s" , 1 }, { "ufi" , 1 }, { "edw" , 3 }, { "ebe" , 1 }, { "nyt" , 2 }, { "cwo" , 1 }, { "esb" , 1 }, { "dme" , 1 }, { "vea" , 4 }, { "eif" , 4 }, { "isn" , 2 }, { "ere" , 33 }, { "ool" , 1 }, { "ecl" , 3 }, { "rds" , 1 }, { "asd" , 2 }, { "rsi" , 11 }, { "itf" , 1 }, { "dev" , 3 }, { "rro" , 1 }, { "het" , 1 }, { "rpl" , 1 }, { "xts" , 1 }, { "nin" , 4 }, { "spo" , 20 }, { "sle" , 2 }, { "hat" , 14 }, { "ksa" , 4 }, { "fcl" , 5 }, { "xta" , 7 }, { "rre" , 4 }, { "ntr" , 2 }, { "nso" , 2 }, { "ryp" , 9 }, { "log" , 2 }, { "ole" , 2 }, { "rdc" , 1 }, { "dus" , 3 }, { "ala" , 3 }, { "rle" , 4 }, { "mos" , 2 }, { "rsl" , 1 }, { "syl" , 2 }, { "gin" , 5 }, { "odm" , 1 }, { "rsm" , 1 }, { "rdf" , 1 }, { "nha" , 1 }, { "atr" , 1 }, { "anf" , 1 }, { "are" , 29 }, { "let" , 45 }, { "rof" , 5 }, { "imp" , 6 }, { "gra" , 3 }, { "suf" , 1 }, { "dbe" , 3 }, { "oke" , 2 }, { "ueh" , 1 }, { "ite" , 6 }, { "elv" , 1 }, { "wte" , 1 }, { "alt" , 2 }, { "toe" , 7 }, { "tht" , 4 }, { "emn" , 1 }, { "lle" , 4 }, { "eya" , 2 }, { "nga" , 1 }, { "cie" , 1 }, { "itl" , 1 }, { "vic" , 1 }, { "yha" , 1 }, { "ksf" , 1 }, { "mec" , 4 }, { "amp" , 17 }, { "tag" , 1 }, { "nci" , 26 }, { "nsc" , 1 }, { "ysp" , 2 }, { "unt" , 2 }, { "lyk" , 1 }, { "geu" , 1 }, { "ics" , 3 }, { "lon" , 1 }, { "seg" , 1 }, { "ara" , 2 }, { "fas" , 1 }, { "ofl" , 4 }, { "yam" , 1 }, { "lop" , 2 }, { "mon" , 3 }, { "ofm" , 3 }, { "nti" , 2 }, { "tun" , 2 }, { "yab" , 1 }, { "odu" , 2 }, { "onb" , 1 }, { "wer" , 6 }, { "blu" , 2 }, { "tdo" , 1 }, { "esk" , 2 }, { "non" , 1 }, { "ige" , 4 }, { "ora" , 3 }, { "ecr" , 2 }, { "deb" , 2 }, { "nor" , 2 }, { "ftd" , 1 }, { "pac" , 3 }, { "rbu" , 1 }, { "eop" , 2 }, { "str" , 5 }, { "nat" , 4 }, { "fco" , 1 }, { "isk" , 1 }, { "yre" , 2 }, { "lyi" , 1 }, { "imi" , 1 }, { "lik" , 3 }, { "inl" , 1 }, { "dif" , 4 }, { "chw" , 1 }, { "ici" , 1 }, { "ond" , 4 }, { "ytr" , 3 }, { "ure" , 2 }, { "hre" , 2 }, { "oes" , 1 }, { "hed" , 2 }, { "ell" , 5 }, { "det" , 6 }, { "muc" , 4 }, { "llo" , 1 }, { "ywo" , 10 }, { "sju" , 2 }, { "ast" , 6 }, { "umn" , 7 }, { "pit" , 1 }, { "esw" , 1 }, { "jum" , 1 }, { "sdo" , 2 }, { "vol" , 4 }, { "uta" , 5 }, { "zar" , 1 }, { "lti" , 2 }, { "ogy" , 2 }, { "bis" , 1 }, { "tch" , 1 }, { "sky" , 2 }, { "kin" , 1 }, { "lyb" , 1 }, { "npl" , 2 }, { "pul" , 1 }, { "lke" , 2 }, { "ini" , 1 }, { "arc" , 5 }, { "afr" , 1 }, { "ova" , 1 }, { "ldn" , 2 }, { "fer" , 5 }, { "van" , 2 }, { "emi" , 3 }, { "ncr" , 7 }, { "sto" , 9 }, { "gmu" , 1 }, { "erb" , 2 }, { "yof" , 1 }, { "egr" , 1 }, { "eno" , 1 }, { "lud" , 3 }, { "chh" , 1 }, { "ysi" , 6 }, { "res" , 19 }, { "kno" , 5 }, { "ced" , 6 }, { "owy" , 2 }, { "esq" , 4 }, { "fci" , 2 }, { "esr" , 2 }, { "ien" , 1 }, { "irs" , 7 }, { "ari" , 1 }, { "tha" , 13 }, { "tfo" , 3 }, { "byt" , 5 }, { "ren" , 7 }, { "ote" , 1 }, { "nyo" , 1 }, { "kli" , 2 }, { "arg" , 1 }, { "sli" , 2 }, { "eai" , 2 }, { "tiv" , 2 }, { "nic" , 3 }, { "vee" , 1 }, { "tho" , 7 }, { "ism" , 3 }, { "ens" , 3 }, { "his" , 13 }, { "uff" , 1 }, { "ste" , 6 }, { "yme" , 1 }, { "def" , 1 }, { "var" , 1 }, { "sna" , 1 }, { "mpl" , 23 }, { "dup" , 1 }, { "yal" , 3 }, { "esy" , 3 }, { "rov" , 1 }, { "rha" , 2 }, { "ati" , 17 }, { "air" , 2 }, { "thc" , 2 }, { "dre" , 2 }, { "ani" , 4 }, { "try" , 1 }, { "dco" , 1 }, { "hma" , 1 }, { "oks" , 1 }, { "ake" , 11 }, { "hwe" , 1 }, { "ume" , 1 }, { "lal" , 1 }, { "alp" , 28 }, { "iex" , 1 }, { "oly" , 3 }, { "tib" , 3 }, { "irp" , 1 }, { "acl" , 1 }, { "sas" , 4 }, { "oml" , 1 }, { "nsl" , 1 }, { "cew" , 1 }, { "che" , 3 }, { "ipe" , 2 }, { "ngi" , 2 }, { "out" , 8 }, { "anc" , 3 }, { "adi" , 1 }, { "eve" , 12 }, { "nyn" , 1 }, { "lte" , 2 }, { "fwo" , 1 }, { "ter" , 47 }, { "ges" , 2 }, { "tsl" , 1 }, { "sym" , 4 }, { "sal" , 4 }, { "pha" , 27 }, { "ius" , 1 }, { "tom" , 1 }, { "iro" , 4 }, { "ocr" , 2 }, { "ese" , 11 }, { "hin" , 6 }, { "ind" , 1 }, { "ech" , 10 }, { "lsf" , 2 }, { "igh" , 3 }, { "oul" , 16 }, { "aza" , 1 }, { "sde" , 3 }, { "dth" , 15 }, { "ldw" , 1 }, { "mak" , 2 }, { "eby" , 1 }, { "dwe" , 1 }, { "omw" , 1 }, { "tod" , 4 }, { "dwr" , 1 }, { "tre" , 2 }, { "uso" , 1 }, { "ofw" , 1 }, { "fre" , 8 }, { "ept" , 4 }, { "reo" , 4 }, { "asw" , 1 }, { "lso" , 6 }, { "hks" , 1 }, { "tsi" , 1 }, { "atw" , 1 }, { "oug" , 2 }, { "hif" , 1 }, { "lty" , 1 }, { "ria" , 1 }, { "cwe" , 1 }, { "llb" , 1 }, { "end" , 2 }, { "tev" , 1 }, { "chm" , 2 }, { "ono" , 3 }, { "xty" , 1 }, { "ela" , 2 }, { "ple" , 27 }, { "oth" , 13 }, { "rss" , 7 }, { "sea" , 2 }, { "cei" , 1 }, { "eit" , 1 }, { "wth" , 1 }, { "edo" , 7 }, { "eni" , 5 }, { "eou" , 1 }, { "tte" , 38 }, { "nen" , 1 }, { "ouh" , 1 }, { "dba" , 1 }, { "cfo" , 2 }, { "rmu" , 1 }, { "emu" , 3 }, { "uth" , 1 }, { "how" , 3 }, { "wid" , 1 }, { "igm" , 1 }, { "owh" , 1 }, { "whe" , 5 }, { "mbo" , 4 }, { "ald" , 1 }, { "woe" , 1 }, { "ndd" , 3 }, { "arl" , 1 }, { "div" , 1 }, { "tby" , 1 }, { "rce" , 2 }, { "enw" , 3 }, { "ows" , 3 }, { "oge" , 1 }, { "eva" , 1 }, { "ead" , 5 }, { "que" , 7 }, { "rti" , 6 }, { "gem" , 2 }, { "mbl" , 2 }, { "ymb" , 4 }, { "ceb" , 1 }, { "iat" , 1 }, { "eri" , 9 }, { "mse" , 2 }, { "yna" , 1 }, { "der" , 17 }, { "eot" , 1 }, { "elf" , 2 }, { "hos" , 3 }, { "lel" , 1 }, { "nwh" , 1 }, { "dte" , 1 }, { "eun" , 1 }, { "isj" , 1 }, { "oba" , 1 }, { "ota" , 1 }, { "lli" , 1 }, { "lda" , 2 }, { "hym" , 1 }, { "lic" , 2 }, { "hep" , 3 }, { "ewi" , 2 }, { "tsu" , 1 }, { "nfr" , 1 }, { "oro" , 4 }, { "cis" , 2 }, { "hth" , 6 }, { "wne" , 1 }, { "dlo" , 2 }, { "nof" , 1 }, { "cel" , 1 }, { "mns" , 2 }, { "ged" , 2 }, { "dfr" , 1 }, { "eca" , 6 }, { "eel" , 1 }, { "ous" , 4 }, { "sec" , 10 }, { "ner" , 5 }, { "lit" , 2 }, { "sob" , 2 }, { "mri" , 1 }, { "eak" , 5 }, { "dbb" , 1 }, { "was" , 3 }, { "hmo" , 1 }, { "pol" , 4 }, { "fyo" , 1 }, { "mno" , 1 }, { "isi" , 6 }, { "xte" , 2 }, { "put" , 1 }, { "len" , 2 }, { "esa" , 18 }, { "orl" , 2 }, { "ett" , 41 }, { "lde" , 2 }, { "efr" , 4 }, { "sci" , 2 }, { "hro" , 2 }, { "maw" , 1 }, { "hle" , 2 }, { "toh" , 1 }, { "awe" , 1 }, { "eke" , 8 }, { "dac" , 1 }, { "met" , 9 }, { "stt" , 1 }, { "shg" , 1 }, { "ths" , 3 }, { "sap" , 1 }, { "eju" , 1 }, { "teo" , 2 }, { "pre" , 5 }, { "yci" , 1 }, { "uar" , 4 }, { "sbi" , 1 }, { "uha" , 1 }, { "thi" , 13 }, { "rth" , 12 }, { "rgr" , 2 }, { "bei" , 2 }, { "tma" , 1 }, { "men" , 2 }, { "tle" , 6 }, { "eag" , 2 }, { "ris" , 4 }, { "tan" , 12 }, { "ort" , 6 }, { "hse" , 1 }, { "fap" , 1 }, { "ywa" , 1 }, { "kci" , 1 }, { "dst" , 1 }, { "leo" , 7 }, { "lfc" , 1 }, { "wik" , 2 }, { "uch" , 11 }, { "wci" , 1 }, { "oha" , 1 }, { "hic" , 5 }, { "lin" , 5 }, { "lbe" , 1 }, { "tkn" , 1 }, { "eth" , 32 }, { "saz" , 1 }, { "nwo" , 2 }, { "xam" , 18 }, { "dei" , 2 }, { "edm" , 3 }, { "beu" , 3 }, { "rop" , 1 }, { "oac" , 1 }, { "xal" , 1 }, { "eda" , 1 }, { "hei" , 5 }, { "avi" , 3 }, { "hta" , 1 }, { "ofa" , 2 }, { "ike" , 3 }, { "par" , 2 }, { "opo" , 1 }, { "lkn" , 1 }, { "con" , 2 }, { "eew" , 1 }, { "utn" , 1 }, { "tnu" , 1 }, { "ack" , 14 }, { "ric" , 3 }, { "vio" , 1 }, { "mem" , 2 }, { "pen" , 1 }, { "edr" , 1 }, { "tes" , 1 }, { "rig" , 6 }, { "dsy" , 1 }, { "dia" , 5 }, { "fus" , 1 }, { "mat" , 2 }, { "yxy" , 1 }, { "dsu" , 1 }, { "edl" , 1 }, { "odf" , 3 }, { "sco" , 1 }, { "dch" , 1 }, { "opl" , 2 }};


int count(string &pat, string &txt) 
{ 
    int M = pat.length(); 
    int N = txt.length(); 
    int res = 0; 
    
    for (int i = 0; i <= N - M; i++) 
    {  
        int j; 
        for (j = 0; j < M; j++) 
            if (txt[i+j] != pat[j]) 
                break; 
   
        if (j == M)   
        { 
           res++; 
           j = 0; 
        } 
    } 
    return res; 
}

/** 
	Подсчет количества триграм
	\param [in] msg сообщение
*/
int trigram_count(string msg){
	return msg.length() - 2;
}

/** 
	Подсчет частоты триграммы
	\param [in] msg сообщение
	\param [in] trig триграмма
*/
double trigram_freq(string msg, string trig){
	int tc = count(trig, msg);
	// if (tc){
	// 	cout << trig << " " << tc << endl;
	// }
	int ct = trigram_count(msg);
	// cout << ct << endl; 
	return tc / double(ct);
}

/** 
	Подсчет совпадения триграмм
	\param [in] msg сообщение
*/
double trigram_fit(string msg){
	double sum = 0;
	for(auto & x : ENGLISH_TRIGRAMS){
		sum += trigram_freq(msg, x.first);
	}

	return sum;
}

/** 
	Сравнение на основе совпадения триграмм
	\param [in] msgа сообщение 1
	\param [in] msgb сообщение 2
*/
static bool trigram_fit_compare(string msga, string msgb){
	// cout << msga.first << endl;
	return trigram_fit(msga) < trigram_fit(msgb);
	// return true;
}


/** 
	Подсчет букв
	\param [in] msg сообщение 
*/
map<char, uint> letterscount(string msg){
	map<char, uint> result = {{ 'a', 0 }, { 'b', 0 }, { 'c', 0 }, { 'd', 0 }, { 'e', 0 }, { 'f', 0 }, { 'g', 0 }, { 'h', 0 }, { 'i', 0 }, { 'j', 0 }, { 'k', 0 }, { 'l', 0 }, { 'm', 0 }, { 'n', 0 }, { 'o', 0 }, { 'p', 0 }, { 'q', 0 }, { 'r', 0 }, { 's', 0 }, { 't', 0 }, { 'u', 0 }, { 'v', 0 }, { 'w', 0 }, { 'x', 0 }, { 'y', 0 }, { 'z', 0 }};
	for (int i = 0; i < msg.length(); ++i)
	{
		if(isalpha(msg[i])){
			result[msg[i]] += 1;
		}
	}

	return result;
}


/** 
	Подсчет indexcoincidence
	\param [in] msg сообщение
*/

double indexcoincidence(string msg){
	auto lettersnumbers = letterscount(msg);
	uint n = msg.length();

	double sum = 0;
	for (char ch = 'a'; ch <= 'z'; ++ch)
	{
		sum += lettersnumbers[ch] * (double(lettersnumbers[ch] - 1) / (n * (n - 1)));
	}

	return sum;
}

/** 
	Представление в виде столбцов
	\param [in] msg сообщение
	\param [in] step шаг
	\param [in] start старт
*/
string columnrepresentation(string msg, uint step, uint start){
	string res = "";

	for (int i = start; i < msg.length(); i += step)
	{
		res += msg[i];
	}

	return res;
}

map<uint, double> shiftedindexcoincidence(string msg){
	map<uint, double> result;
	for (int i = 1; i < 26; ++i)
	 {
	 	string mes = columnrepresentation(msg, i, 0);
	 	result.insert(make_pair(i, indexcoincidence(mes)));
	 } 

	return result;
}

/** 
	Поиск длины ключа шифра Виженера
	\param [in] chiphertext зашифрованное сообщение
	\param [in] step шаг
	\param [in] start старт
*/
uint findvigenerekeylength(string chiphertext, double serchborder){
	auto sc = shiftedindexcoincidence(chiphertext);
	uint min = 100;
	double ENGLISH_CI = 0.0667;
	for (auto i = sc.begin(); i != sc.end(); ++i)
	{
		uint a = i->first;
		double b = i->second;
		if (abs(b - ENGLISH_CI) < ENGLISH_CI * serchborder) {
			if (a < min){
				min = a;
			}
		}	
	}

	return min; 
}

/** 
	Расшифровка цезаря
	\param [in] chiphertext зашифрованное сообщение
	\param [in] step шаг
*/
string caesar_decrypt(string chiphertext, uint step){
	string plaintext(chiphertext);

	for (int i = 0; i < plaintext.length(); ++i)
	{
		if (isalpha(chiphertext[i]))
			plaintext[i] = (52 + chiphertext[i] - 'a' - step) % 26 + 'a';
		else
			plaintext[i] = chiphertext[i];
	}

	return plaintext;
}

/** 
	Частота букв
	\param [in] msg сообщение
*/
map<char, double> frequency(string msg){
	auto lettersnumbers = letterscount(msg);
	map<char, double> result = {{ 'a', 0 }, { 'b', 0 }, { 'c', 0 }, { 'd', 0 }, { 'e', 0 }, { 'f', 0 }, { 'g', 0 }, { 'h', 0 }, { 'i', 0 }, { 'j', 0 }, { 'k', 0 }, { 'l', 0 }, { 'm', 0 }, { 'n', 0 }, { 'o', 0 }, { 'p', 0 }, { 'q', 0 }, { 'r', 0 }, { 's', 0 }, { 't', 0 }, { 'u', 0 }, { 'v', 0 }, { 'w', 0 }, { 'x', 0 }, { 'y', 0 }, { 'z', 0 }};
	for (auto i = lettersnumbers.begin(); i != lettersnumbers.end(); ++i)
	{
		result[i->first] = double(i->second) / msg.length(); 
	}

	return result;
}

/** 
	Корреляция алфавита
	\param [in] msg_frequency сообщение
*/
double alphabetcorrelation(map<char, double> msg_frequency){
	map<char, double> ENGLISH_FREQUESNCY = {{ 'e', 0.12702 }, { 't', 0.09056 }, { 'a', 0.08167 }, { 'o', 0.07507 }, { 'i', 0.06966 }, { 'n', 0.06749 }, { 's', 0.06327 }, { 'h', 0.06094 }, { 'r', 0.05987 }, { 'd', 0.04253 }, { 'l', 0.04025 }, { 'c', 0.02782 }, { 'u', 0.02758 }, { 'm', 0.02406 }, { 'w', 0.0236 }, { 'f', 0.02228 }, { 'g', 0.02015 }, { 'y', 0.01974 }, { 'p', 0.01929 }, { 'b', 0.01492 }, { 'v', 0.00978 }, { 'k', 0.00772 }, { 'j', 0.00153 }, { 'x', 0.0015 }, { 'q', 0.00095 }, { 'z', 0.00074 }};
	double sum = 0;
	for (char i = 'a'; i <= 'z'; ++i)
	{
		sum+= ENGLISH_FREQUESNCY[i] * msg_frequency[i];
	}

	return sum;
}

/** 
	Взлом цезаря
	\param [in] chiphertext зашифрованное сообщение
*/
char breakcaesar(string chiphertext){
	char var = 0;
	double max = -1;
	for (int i = 0; i < 26; ++i)
	{
		auto d = caesar_decrypt(chiphertext, i);
		auto msg_frequency = frequency(d);
		if (alphabetcorrelation(msg_frequency) > max){
			var = i;
			max = alphabetcorrelation(msg_frequency);
		}
	}

	return var;
}

/** 
	Восстановление ключа шифра Виженера
	\param [in] chiphertext зашифрованное сообщение
	\param [in] keylength длина ключа
*/
string recovervigenerekey(string chiphertext, uint keylength){
	string result;
	for (int i = 0; i < keylength; ++i)
	{
		auto msg = columnrepresentation(chiphertext, keylength, i);
		char tmp = breakcaesar(msg);
		string a = "a";
		a[0] = tmp + 'a';
		result.append(a);
	}

	return result;
}

/** 
	Шифрование Виженера
	\param [in] pl сообщение
	\param [in] key ключ
*/
string viziner_encrypt(string pl, string key){ // шифрование виженер
	int l = key.length();
	string chiphertext(pl);
	for (int i = 0; i < pl.length(); ++i)
	{
		if (isalpha(pl[i])){
			chiphertext[i] = (key[i % l] - 'a' + pl[i] - 'a') % 26 + 'a';
		} else {
			chiphertext[i] = pl[i];
		}
	}
	return chiphertext;
}

/** 
	Дешифрование Виженера
	\param [in] ch зашифрованное сообщение
	\param [in] key ключ
*/
string viziner_decrypt(string ch, string key){ // шифрование виженер
	int l = key.length();
	string plaintext(ch);
	for (int i = 0; i < ch.length(); ++i)
	{
		if (isalpha(ch[i])){
			plaintext[i] = (-(key[i % l] - 'a') + ch[i] - 'a' + 26) % 26 + 'a';
		} else {
			plaintext[i] = ch[i];
		}
	}
	return plaintext;
}

/** 
	Генерация ключа шифра подстановки
*/
string substitute_generate_key(){ // генерация ключа замены
	string alphabet = "abcdefghijklmnopqrstuvwxyz";
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	shuffle(alphabet.begin(), alphabet.end(), default_random_engine(seed));
	return alphabet;
}

/** 
	Шифрование Подстановки
	\param [in] plaintext сообщение
	\param [in] key ключ
*/
string substitute_encrypt(string plaintext, string key){ // шифрование замены
	string chiphertext;
	for (int i = 0; i < plaintext.length(); ++i)
	{
		if (isalpha(plaintext[i])){
			string a = "a";
			// cout << int(plaintext[i] - 'a') << ' ';
			a[0] = key[plaintext[i] - 'a'];
			chiphertext.append(a); 
		}
	}

	return chiphertext;
}

/** 
	Дешифрование Подстановки
	\param [in] chiphertext зашифрованное сообщение
	\param [in] key ключ
*/
string substitute_decrypt(string chiphertext, string key){ //расшифровка замены
	string plaintext;
	// cout << chiphertext.length();
	for (int i = 0; i < chiphertext.length(); ++i)
	{
		string let = chiphertext.substr(i, 1);
		int index = key.find(let);
		// cout << index << ' ';
		let = "a";
		let[0] = 'a' + index;
		plaintext.append(let);
	}

	return plaintext;
}

/** 
	Взлом Подстановки
	\param [in] ch зашифрованное сообщение
*/
pair<string,string> substitute_break(string ch){
	srand(chrono::system_clock::now().time_since_epoch().count());
	vector<pair<string,string>> result;
	for (int i = 0; i < 10; ++i)
	{
		string parentkey = substitute_generate_key();
		string d = substitute_decrypt(ch, parentkey);
		double fit = trigram_fit(d);
		int count = 0;
		while (count < 2000){
			cerr << parentkey << endl;
			int i = rand() % 25;
			int j = rand() % 25;
			string childkey(parentkey);
			swap(childkey[i], childkey[j]);
			d = substitute_decrypt(ch, childkey);
			if (trigram_fit(d) > fit) {
				parentkey = childkey;
				fit = trigram_fit(d);
				count = 0;
			}
			count += 1;
		}
		result.push_back(make_pair(parentkey, substitute_decrypt(ch, parentkey)));
	}

	auto ret_i = result[0];
	for (auto i = result.begin(); i != result.end(); ++i)
	{
		if(trigram_fit_compare(ret_i.second, i->second)){
			ret_i = *i;
		}
	}
	return ret_i;
}

/** 
	Генерация ключа шифра полибия
*/
string polibiy_generate_key(){
	string alphabet = "abcdefghiklmnopqrstuvwxyz";
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	shuffle(alphabet.begin(), alphabet.end(), default_random_engine(seed));
	return alphabet; // генерациия ключа полибий
}

/** 
	Шифрование Подстановки
	\param [in] pl сообщение
	\param [in] key ключ
*/
string polibiy_encrypt(string pl, string key){
	string chiphertext;
	for (int i = 0; i < pl.length(); ++i)
	{
		string help("AA ");
		string let = pl.substr(i, 1);
		if (isalpha(let[0])) {
			int index = key.find(let);
			help[0] = (index / 5) + 'A';
			help[1] = (index % 5) + 'A';
			chiphertext.append(help);
		}
	}

	return chiphertext;
}
/** 
	Дешифрование Полибия
	\param [in] ch зашифрованное сообщение
	\param [in] key ключ
*/
string polibiy_decrypt(string ch, string key){
	string plaintext;
	for (int i = 0; i < ch.length(); i += 3)
	{
		string help = ch.substr(i, 3);
		// cout << help << endl;
		int index  = (help[0] - 'A') * 5;
		index += help[1] - 'A';
		string let = key.substr(index, 1);
		plaintext.append(let);
	}

	return plaintext;
}

/** 
	Взлом Полибия
	\param [in] ch зашифрованное сообщение
*/
pair<string, string> polibiy_break(string ch){
	srand(chrono::system_clock::now().time_since_epoch().count());
	vector<pair<string, string>> result;
	for (int i = 0; i < 10; ++i)
	{
		string parentkey = polibiy_generate_key();
		string d = polibiy_decrypt(ch, parentkey);
		double fit = trigram_fit(d);
		int count = 0;
		while (count < 1500){
			// cerr << parentkey << endl;
			int i = rand() % 25;
			int j = rand() % 25;
			string childkey(parentkey);
			swap(childkey[i], childkey[j]);
			d = polibiy_decrypt(ch, childkey);
			if (trigram_fit(d) > fit) {
				parentkey = childkey;
				fit = trigram_fit(d);
				count = 0;
			}
			count += 1;
		}
		result.push_back(make_pair(parentkey, polibiy_decrypt(ch, parentkey)));
	}

	auto ret_i = result[0];
	for (auto i = result.begin(); i != result.end(); ++i)
	{
		if(trigram_fit_compare(ret_i.second, i->second)){
			ret_i = *i;
		}
	}
	return ret_i;
}



int main(int argc, char const *argv[])
{
	string plaintext = "a cipher is a means of concealing a message, where letters of the message are substituted or transposed for other letters, letter pairs, and sometimes for many letters. in cryptography, a classical cipher is a type of cipher that was used historically but now has fallen, for the most part, into disuse. in general, classical ciphers operate on an alphabet of letters (such as a-z), and are implemented by hand or with simple mechanical devices. they are probably the most basic types of ciphers, which made them not very reliable, especially after new technology was developed. modern schemes use computers or other digital technology, and operate on bits and bytes. many classical ciphers were used by well-respected people, such as iulius caesar and napoleon, who created their own ciphers which were then popularly used. many ciphers had their origins in the military and were used for transporting secret messages among people on the same side. classical schemes are often susceptible to ciphertext-only attacks, sometimes even without knowledge of the system itself, using tools such as frequency analysis. sometimes grouped with classical ciphers are more advanced mechanical or electro-mechanical cipher machines, such as the enigma machine.";

	int choice = 0;

	while (true){
		cout << "Выберите атакуемый шифр: \n1) Шифр Полибия\n2) Шифр замены\n3) Шифр Виженера\n4) des" << endl;
		cin >> choice;

		switch (choice){
			case 1: {
				string key = polibiy_generate_key();
				cout << "Сгенерированный ключ: " << key << endl;   
				cout << "Незашифрованный текст: \n" << plaintext << endl;
				string chiphertext = polibiy_encrypt(plaintext, key);
				cout << "Зашифрованный текст: \n" << chiphertext << endl;
				pair<string, string> break_result = polibiy_break(chiphertext);
				cout << "Раскрытый ключ: " << break_result.first << endl << "Раскрытый текст: \n" << break_result.second << endl;
				if (key == break_result.first)
				{
					cout << "Атака прошла успешно" << endl;
				} else {
					cout << "Атака прошла неуспешно" << endl;		
				}
				break;
			}
			case 2: {
				string key = substitute_generate_key();
				cout << "Сгенерированный ключ: " << key << endl;   
				cout << "Незашифрованный текст: \n" << plaintext << endl;
				string chiphertext = substitute_encrypt(plaintext, key);
				cout << "Зашифрованный текст: \n" << chiphertext << endl;
				cout << substitute_decrypt(chiphertext, key) << endl;
				pair<string, string> break_result = substitute_break(chiphertext);
				cout << "Раскрытый ключ: " << break_result.first << endl << "Раскрытый текст: \n" << break_result.second << endl;
				if (key == break_result.first)
				{
					cout << "Атака прошла успешно" << endl;
				} else {
					cout << "Атака прошла неуспешно" << endl;		
				}
				break;
			}
			case 3: {
				string key;
				cout << "Введите ключ на английском языке (строчными буквами)" << endl;
				cin >> key;
				cout << "Незашифрованный текст: \n" << plaintext << endl;
				string chiphertext = viziner_encrypt(plaintext, key);
				cout << "Зашифрованный текст: \n" << chiphertext << endl;
				string break_key = recovervigenerekey(chiphertext, findvigenerekeylength(chiphertext, 0.4));
				cout << "Раскрытый ключ: " << break_key << endl;
				cout << "Раскрытый текст: \n" << viziner_decrypt(chiphertext, break_key) << endl;
				if (break_key.find(key) == 0)
				{
					cout << "Атака прошла успешно" << endl;
				} else {
					cout << "Атака прошла неуспешно" << endl;		
				}
				break;
			}
			case 4: {

				uchar * des_src = (uchar *) "Михаил Михайлович Пришвин.Идёт человек по лесу. На деревьях тяжёлые шапки. Ёлки выпрямились. Сильные еловые лапы держат снег. А берёзы согнулись дугой. Человек стукнул палкой по берёзе. Снег с вершины свалился. Русская красавица свободно вздохнула, выпрямилась.Так от одной берёзки к другой идёт писатель Пришвин. Он освобождает деревья от снежного плена. (48 слов)Лакомка.Стоял тёплый сентябрьский день. Странный шум доносился из молодого ельника. Мы осторожно раздвинули ёлочки. На земле сидел крупный ястреб. Своими когтистыми лапами он прижимал к земле кусок пчелиных сот. Пчёлы атаковали пернатого разбойника. Ястреб спокойно лакомился мёдом. Вот он медленно взмахнул могучими крыльями и полетел в чащу. В когтях он держал соты. (52 слова) Луковица с радостью.Папа привёз мне с юга большую луковицу. Он сказал, что в середине этой луковицы спрятана радость.Я удивилась. В такой серой луковице и вдруг – радость! Мама посадила мою луковицу в горшочек и отнесла в подполье. Прошло много дней. И вот однажды мама подает мне горшочек, а там остренький желтый торчок. Мы поставили горшочек на окно.И перед самым праздником он расцвел. Сколько у него было цветов – синих-синих, душистых-душистых! Как красив был каждый цветок! Папа сказал, что мой цветок зовут гиацинтом. Но я стала называть его Гиней.И я полюбила его больше всех своих игрушек. Игрушки только понарошку живые, а цветок – живой по-настоящему. (102 словЁлка В гостиную втащили большую мёрзлую ёлку. От неё веяло холодом, но понемножку слежавшиеся ветки её оттаяли. Она поднялась, распушилась. По всему дому запахло хвоей. Дети принесли коробки с украшениями, подставили к ёлке стул и стали её наряжать. Её опутали золотой паутинкой, повесили серебряные цепи, поставили свечи. Она вся блестела, переливалась золотом, искрами, длинными лучами. Свет от неё шёл густой, тёплый, пахнущий хвоей. (63 слова) Елка.Было ясное морозное утро. Окна покрылись густым слоем снежных листьев и цветов. В комнату внесли большую елку. От елки потянуло холодком. Но вот веточки ее оттаяли, распушились. В доме запахло хвоей. Ребята стали наряжать елку. На лесной красавице появились хлопушки, бусы, длинные бумажные цепи. От веточки к веточке потянулись золотые паутинки. На самой верхушке елки мальчики Кирилл и Геннадий укрепили звезду. Елка ожила, засветилась огнями. (65 слов)";
				uchar des_dst[4312];
				ulong64 key = {32495889148, 3584860338ul};
				des_encrypt_ecb(des_src, strlen((char *)des_src), des_dst, key);
				ulong break_key = des_break(des_src, strlen((char *)des_src), des_dst);
				cout << "Незашифрованный текст: \n" << des_src << endl;
				cout << "key_1[25..30] = " << break_key << endl;
				if ((key0.l << 2) >> 26 == break_key)
				{
					cout << "Атака прошла успешно" << endl;
				} else {
					cout << "Атака прошла неуспешно" << endl;		
				}
				break;
			}
			default:
				cout << "Неверный выбор. Попробуйте еще раз" << endl;
				continue;
		}
	}


	return 0;
}
