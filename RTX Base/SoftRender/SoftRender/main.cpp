#include "Render.hpp"
#include "ray.h"
#include "camera.h"
#include "hitable_list.h"
#include "sphere.h"

#define random(a,b) (rand()%(b-a+1)+a)
#define HEIGHT 50
#define WIDTH 100
#define MAXFLOAT FLT_MAX
#define SRGB(r, g, b) (r) << 16 | (g) << 8 | b

vec3 color(const ray& r, hitable* world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec))
	{
		ray scattered;
		// Ë¥¼õ
		vec3 attenuation;

		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			// µÝ¹é Ë¥¼õ
			return attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 *(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

void Render(device_t* device)
{
	int ns = 2;

	vec3 lf(-2.0, -1.0, -1.0);
	vec3 h(4.0, 0.0, 0.0);
	vec3 vt(0.0, 2.0, 0.0);
	vec3 o(0.0, 0.0, 0.0);

	hitable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);

	hitable* world = new hitable_list(list, 2);

	for (int j = HEIGHT - 1; j >= 0; j--)
	{
		for (int i = 0; i < WIDTH; ++i)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = (float(i) + float(random(0, 100)) / 100.0f) / float(WIDTH);
				float v = (float(j) + float(random(0, 100)) / 100.0f) / float(HEIGHT);

				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			device->DrawPixel(i, j, ir, ig, ib);
		}
	}
}

int main()
{
	device_t* pDevice = InitNewDevice(WIDTH, HEIGHT, "Ray");
	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) 
	{
		screen_dispatch();
		device_clear(pDevice, 1);
		Render(pDevice);
		screen_update();
		Sleep(1);
	}
	return 0;
}