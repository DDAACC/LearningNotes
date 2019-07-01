#pragma once
#include "hitable.h"
class material {
public:
	// 散射虚函数
	// 参数：r_in 入射的光线， rec hit的记录， attenuation v3的衰减，scattered 散射后的光线
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class metal : public material {
public:
	metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuzz;
};