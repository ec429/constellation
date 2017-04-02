#include "plot.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

void draw_orbits(SDL_Surface *s, unsigned int norbits, struct orbit *orbits);
void draw_markers(SDL_Surface *s, unsigned int nsats, struct sat *sats);
void shade_zones(SDL_Surface *s, unsigned int nsats, struct sat *sats, double range, double *minlat);
double calculate_range(double range);

#define NSATS 12
#define EARTH_RADIUS	6371
#define SAT_RANGE	12000
#define min(x, y)	((x) < (y) ? (x) : (y))

int main(void)
{
	SDL_Surface *screen = ginit(480, 240);
	bool running = false;
	bool show_zones = true;
	double range = 4000.0;
	double theta = 0;
	double inc = 28.6; // Cape latitude
	double minlat = 0;
	int errupt = 0;
	int i;

	struct orbit orbits[3] = {
		{ .radius = EARTH_RADIUS + 4170.0,
		  .inc = inc / 90.0,
		  .phi_by_tau = 0,
		  .rgb = RED },
		{ .radius = EARTH_RADIUS + 4170.0,
		  .inc = inc / 90.0,
		  .phi_by_tau = 1/3.0,
		  .rgb = GREEN },
		{ .radius = EARTH_RADIUS + 4170.0,
		  .inc = inc / 90.0,
		  .phi_by_tau = 2/3.0,
		  .rgb = BLUE },
	};

	struct sat sats[NSATS];

	for (i = 0; i < NSATS; i++) {
		sats[i].orbit = orbits + (i % 3);
		sats[i].theta_by_tau = i / 4.0;
	}

	while(!errupt) {
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		for (i = 0; i < NSATS; i++)
			locate_sat(sats + i, theta);
		if (show_zones)
			shade_zones(screen, NSATS, sats, calculate_range(range), &minlat);
		printf("min. lat.: %4.2f\n", minlat * 90.0);
		draw_markers(screen, NSATS, sats);
		draw_orbits(screen, 3, orbits);
		SDL_Flip(screen);
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				errupt++;
				break;
			case SDL_KEYDOWN:
				if (event.key.type == SDL_KEYDOWN) {
					SDL_keysym key = event.key.keysym;
					//fprintf(stderr, "%04x\n", key.sym);
					if (key.sym == SDLK_q)
						errupt++;
					else if (key.sym == SDLK_r)
						running = !running;
					else if (key.sym == SDLK_z)
						show_zones = !show_zones;
					else if (key.sym == SDLK_UP) {
						inc += 0.4;
						for (i = 0; i < 3; i++)
							orbits[i].inc = inc / 90.0;
						printf("inc = %4.1f°\n", inc);
					} else if (key.sym == SDLK_DOWN) {
						inc -= 0.4;
						for (i = 0; i < 3; i++)
							orbits[i].inc = inc / 90.0;
						printf("inc = %4.1f°\n", inc);
					} else if (key.sym == SDLK_RIGHT)
						theta += 1/96.0;
					else if (key.sym == SDLK_LEFT)
						theta += 1 - 1/96.0;
					else if (key.sym == SDLK_0)
						theta = 0;
					else if (key.sym == SDLK_EQUALS) {
						range += (key.mod & KMOD_CTRL ? 100.0 : 1000.0);
						printf("range = %gkm, effective %gkm\n", range, calculate_range(range));
					} else if (key.sym == SDLK_MINUS) {
						range -= (key.mod & KMOD_CTRL ? 100.0 : 1000.0);
						printf("range = %gkm, effective %gkm\n", range, calculate_range(range));
					}
				}
				break;
			}
		}
		SDL_Delay(50);
		if (running)
			theta += 1/384.0;
	}
	return 0;
}

void draw_orbits(SDL_Surface *s, unsigned int norbits, struct orbit *orbits)
{
	unsigned int i;

	for (i = 0; i < norbits; i++)
		plot_orbit(s, orbits + i);
}

void draw_markers(SDL_Surface *s, unsigned int nsats, struct sat *sats)
{
	unsigned int i;

	for (i = 0; i < nsats; i++)
		plot_location(s, sats[i].loc_s, sats[i].orbit->rgb);
}

void shade_zones(SDL_Surface *s, unsigned int nsats, struct sat *sats, double range, double *minlat)
{
	int x, y;

	*minlat = 1.0;
	for (x = 0; x < s->w; x++) {
		double px = x / (double)s->w;
		for (y = 0; y < s->h; y++) {
			unsigned char rgb[3] = {0};
			double py = 1.0 - (y / (double)s->h) * 2.0;
			struct rv3 r = rv3_from_sv3((struct sv3){
				.r = EARTH_RADIUS,
				.phi = px * M_PI * 2.0,
				.theta = py * M_PI / 2.0,
				});
			unsigned int i;

			for (i = 0; i < nsats; i++) {
				struct rv3 dr = sub_rv3(sats[i].loc_r, r);
				double dot = dot_rv3(dr, r);
				double mags = dot_rv3(dr, dr);
				int j;

				if (dot <= 0)
					continue;
				if (mags > range*range)
					continue;
				for (j = 0; j < 3; j++)
					if (sats[i].orbit->rgb[j])
						rgb[j] += 32;
			}
			if (!rgb[0] && !rgb[1] && !rgb[2])
				*minlat = min(*minlat, fabs(py));
			pset(s, x, y, rgb);
		}
	}
}

double calculate_range(double range)
{
	return range + sqrt(range * SAT_RANGE);
}
