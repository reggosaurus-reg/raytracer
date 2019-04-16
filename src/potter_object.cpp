struct Object;
struct RayHit
{
	Vector point;
	Vector normal;
	Object *object;
};

struct Object
{
	/*
	float roughness;
	*/
	Pixel color;

	virtual RayHit intersect(Vector origin, Vector ray) = 0;
};

struct Sphere : public Object
{
	Vector position;
	float r;

	virtual RayHit intersect(Vector origin, Vector ray) 
	{
		Vector closest_point = origin + ray * dot(ray, position - origin);
		float dist_sqrd = length_sq(closest_point - position);
		float radius_sqrd = r * r;
		if (dist_sqrd <= radius_sqrd) {
			float q = sqrt(radius_sqrd - dist_sqrd);
			Vector intersection_a = closest_point - ray * q;
			Vector intersection_b = closest_point + ray * q;
			Vector intersection;

			if (dot(ray, intersection_a) < dot(ray, intersection_b))
				intersection = intersection_a;
			else
				intersection = intersection_b;

			// Is object in front of ray?
			if (dot(ray, intersection - origin) > 0)
				return {intersection, normalize(intersection - position), this};
		}
		return {};
	}
};

Sphere make_sphere(float x, float y, float z, float r)
{
	Sphere s;
	s.position.x = x;
	s.position.y = y;
	s.position.z = z;
	s.r = r;
	return s;
}

struct Plane : public Object
{
	Vector normal;
	float d;

	virtual RayHit intersect(Vector origin, Vector ray) 
	{
		Vector point; 
		float distance_from_origin = d - dot(origin, normal) ;
		point = origin + ray * (distance_from_origin / dot(ray, normal));
		return {point, -normal, this};
	}
};

Plane make_plane(float x, float y, float z, float d)
{
	// TODO: Make something smart here.
	Plane p;
	p.normal = normalize(V3(x, y, z));
	p.d = d;
	return p;
}

