#include <stdlib.h>
#include <stdbool.h>
#include "util.h"

bool CheckCol(Box box1, Box box2)
{
	return (abs(box1.x - box2.x) * 2 < (box1.width + box2.width)) &&
		(abs(box1.y - box2.y) * 2 < (box1.height + box2.height));
}