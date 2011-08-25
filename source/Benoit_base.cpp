//(c) Jack Hall 2011, licensed under GNU LGPL v3

#include "Benoit_base.h"

ostream& operator<<(ostream &out, const Benoit_base& cBenoit_base) {
	return print(out); 		//defined in all child classes
}
