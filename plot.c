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

void plot_sine(SDL_Surface *s, double a, double phi, const unsigned char *rgb)
{
	int x;

	if (!s) return;
	for (x = 0; x < s->w; x++) {
		double w = x * 2.0 * M_PI / s->w;
		double y = (1.0 - a * sin(w - phi)) * s->h / 2.0;

		pset(s, x, y + 0.5, rgb);
	}
}

void plot_sine_frac(SDL_Surface *s, double a, double phi_by_tau, const unsigned char *rgb)
{
	plot_sine(s, a, phi_by_tau * 2.0 * M_PI, rgb);
}

void plot_marker(SDL_Surface *s, double a, double phi, double theta, double r, const unsigned char *rgb)
{
	double x = fmod(theta / M_PI / 2.0 + 1, 1.0);
	double y = a * sin(phi + theta);
	plot_location(s, x, y, r, rgb);
}


void plot_marker_frac(SDL_Surface *s, double a, double phi_by_tau, double theta_by_tau, double r, const unsigned char *rgb)
{
	plot_marker(s, a, phi_by_tau * M_PI * 2.0, theta_by_tau * M_PI * 2.0, r, rgb);
}

void plot_orbit(SDL_Surface *s, struct orbit *orbit)
{
	plot_sine_frac(s, orbit->inc, orbit->phi_by_tau, orbit->rgb);
}

void locate_sat(struct sat *sat, double theta_offset_by_tau)
{
	double theta_by_tau = sat->theta_by_tau + theta_offset_by_tau;
	sat->x = fmod(theta_by_tau + sat->orbit->phi_by_tau + 1, 1.0);
	sat->y = sat->orbit->inc * sin(theta_by_tau * M_PI * 2.0);
}

void plot_location(SDL_Surface *s, double x, double y, double r, const unsigned char *rgb)
{
	double sx = x * s->w;
	double sy = (1.0 - y) * s->h / 2.0;
	int dx, dy, dt;

	for (dy = -2; dy < 3; dy++)
		for (dx = -2; dx < 3; dx++)
			pset(s, sx + dx, sy + dy, rgb);

	for (dt = 0; dt < 72; dt++) {
		double dx = r * cos(dt * M_PI / 36.0);
		double dy = r * sin(dt * M_PI / 36.0);
		pset(s, sx + dx, sy + dy, rgb);
	}
}
