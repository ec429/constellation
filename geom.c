#include "geom.h"

struct sv3 sv3_from_rv3(struct rv3 r)
{
	struct sv3 sv;

	sv.r = hypot(hypot(r.x, r.y), r.z);
	sv.theta = acos(r.z / sv.r);
	sv.phi = atan2(r.y, r.x);
	return sv;
}

struct rv3 rv3_from_sv3(struct sv3 s)
{
	struct rv3 rv;

	rv.x = s.r * sin(s.theta) * cos(s.phi);
	rv.y = s.r * sin(s.theta) * sin(s.phi);
	rv.z = s.r * cos(s.theta);
	return rv;
}

struct rv3 rotate_z(struct rv3 r, double phi)
{
	struct rv3 rv;

	rv.x = r.x * cos(phi) + r.y * sin(phi);
	rv.y = r.y * cos(phi) - r.x * sin(phi);
	rv.z = r.z;
	return rv;
}

struct rv3 rotate_y(struct rv3 r, double theta)
{
	struct rv3 rv;

	rv.x = r.x * cos(theta) + r.z * sin(theta);
	rv.y = r.y;
	rv.z = r.z * cos(theta) - r.x * sin(theta);
	return rv;
}
