#include <SDL.h>
#include "geom.h"

#define RED	((const unsigned char[3]){128, 0, 0})
#define GREEN	((const unsigned char[3]){0, 128, 0})
#define BLUE	((const unsigned char[3]){0, 0, 192})

SDL_Surface *ginit(int x, int y);
void pset(SDL_Surface *s, int x, int y, const unsigned char *rgb);

struct orbit {
	double radius;
	double inc;
	double phi_by_tau;
	const unsigned char *rgb;
};

void plot_orbit(SDL_Surface *s, struct orbit *orbit);

struct sat {
	struct orbit *orbit;
	double theta_by_tau;
	/* Last computed location */
	struct rv3 loc_r;
	struct sv3 loc_s;
};

void locate_sat(struct sat *sat, double theta_offset_by_tau);
void plot_location(SDL_Surface *s, struct sv3 loc, const unsigned char *rgb);
