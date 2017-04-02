#include "plot.h"
#include <stdbool.h>

void draw_orbits(SDL_Surface *s, unsigned int norbits, struct orbit *orbits);
void draw_markers(SDL_Surface *s, unsigned int nsats, struct sat *sats, double r);
void shade_zones(SDL_Surface *s, unsigned int nsats, struct sat *sats, double r);

int main(void)
{
	SDL_Surface *screen = ginit(640, 320);
	bool running = false;
	double theta = 0;
	double inc = 0.8;
	double r = 100;
	int errupt = 0;
	int i;

	struct orbit orbits[3] = {
		{ .inc = inc, .phi_by_tau = 0,     .rgb = RED },
		{ .inc = inc, .phi_by_tau = 1/3.0, .rgb = GREEN },
		{ .inc = inc, .phi_by_tau = 2/3.0, .rgb = BLUE },
	};

	struct sat sats[12];

	for (i = 0; i < 12; i++) {
		sats[i].orbit = orbits + (i % 3);
		sats[i].theta_by_tau = (i / 3) / 4.0;
	}

	while(!errupt) {
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		for (i = 0; i < 12; i++)
			locate_sat(sats + i, theta);
		shade_zones(screen, 12, sats, r);
		draw_markers(screen, 12, sats, r);
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
					else if (key.sym == SDLK_UP) {
						inc += 0.04;
						for (i = 0; i < 3; i++)
							orbits[i].inc = inc;
					} else if (key.sym == SDLK_DOWN) {
						inc -= 0.04;
						for (i = 0; i < 3; i++)
							orbits[i].inc = inc;
					} else if (key.sym == SDLK_RIGHT)
						theta += 1/96.0;
					else if (key.sym == SDLK_LEFT)
						theta += 1 - 1/96.0;
					else if (key.sym == SDLK_0)
						theta = 0;
					else if (key.sym == SDLK_EQUALS)
						r *= 1.2;
					else if (key.sym == SDLK_MINUS)
						r /= 1.2;
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

void draw_markers(SDL_Surface *s, unsigned int nsats, struct sat *sats, double r)
{
	unsigned int i;

	for (i = 0; i < nsats; i++)
		plot_location(s, sats[i].x, sats[i].y, r, sats[i].orbit->rgb);
}

void shade_zones(SDL_Surface *s, unsigned int nsats, struct sat *sats, double r)
{
	int x, y;

	for (x = 0; x < s->w; x++) {
		double px = x / (double)s->w;
		for (y = 0; y < s->h; y++) {
			unsigned char rgb[3] = {0};
			double py = 1.0 - (y / (double)s->h) * 2.0;
			unsigned int i;

			for (i = 0; i < nsats; i++) {
				double dx = sats[i].x - px;
				double dy = sats[i].y - py;

				if (dx > 0.5)
					dx -= 1.0;
				if (dx < -0.5)
					dx += 1.0;
				dx *= s->w;
				dy *= s->h / 2.0;
				if (dx*dx + dy*dy < r*r) {
					int j;

					for (j = 0; j < 3; j++)
						if (sats[i].orbit->rgb[j])
							rgb[j] += 16;
				}
			}
			pset(s, x, y, rgb);
		}
	}
}
