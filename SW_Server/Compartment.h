#pragma once
#include "../SW/SW.h"
#include "../SW/Compartment.h"

namespace SW_Server
{
	class Compartment :
		public SW::Compartment
	{
	public:
		Compartment(SW::CompartmentIndex_t index);
		~Compartment();
	};
}

