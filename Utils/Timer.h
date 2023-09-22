#pragma once
#include <cstdint>
#include "vendor/stdafx.h"

namespace MS {

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

		count_type getTimeCount() const { return m_timeCount; }

		value_type getSecPerCount() const { return m_secPerCount;  }
		value_type getTimeBetweenCounts(count_type start, count_type stop) const { 	
			return static_cast<value_type>(stop - start) * m_secPerCount;
		}

	private:

		count_type m_timeCount{};
		value_type m_secPerCount{};

	};

} // -- END NAMESPACE MS
