#include "position.h"

ostream& Position_print( ostream& out, Position p ) {
	out << "(y=" << p._y << ", x=" << p._x << ")";
	return out;
}

ostream& PositionCost_print( ostream& out, PositionCost pc ) {
	out << "(y=" << pc._y << ", x=" << pc._x << ", cost=" << pc._cost << ")";
	return out;
}
