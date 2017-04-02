#include <math.h>

struct rv3 {
	double x, y, z;
};

struct sv3 {
	double r;
	double theta;
	double phi;
};

struct sv3 sv3_from_rv3(struct rv3 r);
struct rv3 rv3_from_sv3(struct sv3 s);
struct rv3 rotate_z(struct rv3 r, double phi);
struct rv3 rotate_y(struct rv3 r, double theta);
struct rv3 sub_rv3(struct rv3 a, struct rv3 b);
double dot_rv3(struct rv3 a, struct rv3 b);
