#include "plot.h"
#include <stdio.h>
#include <math.h>

SDL_Surface *ginit(int x, int y)
{
	SDL_Surface *surf;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		perror("SDL_Init");
		return NULL;
	}
	atexit(SDL_Quit);
	if ((surf = SDL_SetVideoMode(x, y, 32, SDL_HWSURFACE)) == 0) {
		perror("SDL_SetVideoMode");
		SDL_Quit();
		return NULL;
	}
	if (SDL_MUSTLOCK(surf) && SDL_LockSurface(surf) < 0)
		perror("SDL_LockSurface");
	return surf;
}

void pset(SDL_Surface *s, int x, int y, const unsigned char *rgb)
{
	uint32_t pixval;
	size_t s_off;

	if (!s)
		return;
	if ((x < 0) || (y < 0))
		return;
	if ((x >= s->w) || (y >= s->h))
		return;
	s_off = (y * s->pitch) + (x * s->format->BytesPerPixel);
	pixval = SDL_MapRGB(s->format, rgb[0], rgb[1], rgb[2]);
	*(uint32_t *)((char *)s->pixels + s_off) = pixval;
	return;
}

#define ORBIT_POINTS	240

void plot_orbit(SDL_Surface *s, struct orbit *orbit)
{
	struct sat bogosat = {.orbit = orbit, .theta_by_tau = 0};
	double x, y, sx, sy;
	int i;

	for (i = 0; i < ORBIT_POINTS; i++) {
		locate_sat(&bogosat, i / (double)ORBIT_POINTS);
		x = bogosat.loc_s.phi / (M_PI * 2.0);
		if (x < 0.0)
			x += 1.0;
		y = bogosat.loc_s.theta * 2.0 / M_PI;
		sx = x * s->w;
		sy = (1.0 - y) * s->h / 2.0;
		pset(s, sx, sy, orbit->rgb);
	}
}

void locate_sat(struct sat *sat, double theta_offset_by_tau)
{
	double theta_by_tau = sat->theta_by_tau + theta_offset_by_tau;
	struct rv3 pos = {sat->orbit->radius, 0, 0};

	pos = rotate_z(pos, -theta_by_tau * M_PI * 2.0);
	pos = rotate_y(pos, sat->orbit->inc * M_PI / 2.0);
	sat->loc_r = rotate_z(pos, (sat->orbit->phi_by_tau - 0.25) * M_PI * 2.0);
	sat->loc_s = sv3_from_rv3(sat->loc_r);
}

void plot_location(SDL_Surface *s, struct sv3 loc, const unsigned char *rgb)
{
	double x = loc.phi / (M_PI * 2.0);
	double y = loc.theta * 2.0 / M_PI;
	double sx, sy;
	int dx, dy;

	if (x < 0.0)
		x += 1.0;
	sx = x * s->w;
	sy = (1.0 - y) * s->h / 2.0;

	for (dy = -2; dy < 3; dy++)
		for (dx = -2; dx < 3; dx++)
			pset(s, sx + dx, sy + dy, rgb);
}
