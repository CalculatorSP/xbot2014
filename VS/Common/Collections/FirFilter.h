#pragma once

#include "ModBuffer.h"

template <typename T, size_t N>
class FirFilter
{
	float _weights[N];
	ModBuffer _data;

public:
	FirFilter<T, N>(const float weights[N]) : _weights(weights)
	{ }



};