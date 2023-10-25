#pragma once
#include "vendor/stdafx.h"

class Timer
{

public:

	using value_type = double;
	using count_type = long long;

	Timer() 
	{
		LARGE_INTEGER counterFrequency;
		::QueryPerformanceFrequency(&counterFrequency);
		m_secPerCount = 1.0 / static_cast<value_type>(counterFrequency.QuadPart);
	}

	static count_type getTimeCount() { 
		LARGE_INTEGER countNumber;
		::QueryPerformanceCounter(&countNumber);
		return countNumber.QuadPart;
		;
	}

	value_type getSecPerCount() const { return m_secPerCount;  }
	value_type getTimeBetweenCounts(const count_type start, const count_type stop) const { 	
		return static_cast<value_type>(stop - start) * m_secPerCount;
	}

private:

	value_type m_secPerCount{};

};

