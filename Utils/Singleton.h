#pragma once

namespace MS
{

	template <class T> 
	class Singleton
	{
	public:

        static T& getInstance()
        {
			static T Instance;
            return Instance;
        }

		Singleton(const Singleton&) = delete;
		void operator=(const Singleton&) = delete;


	protected :

		Singleton(){}	
		~Singleton(){}


  
	};
}