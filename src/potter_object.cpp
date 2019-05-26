struct Object;
struct RayHit
{
	Vector point;
	Vector normal;
	Object *object;

	Color color;
	float lightness;
};

RayHit empty_hit()
{
	RayHit hit = {};
	hit.color = {0.0f, 0.0f, 0.0f, 1.0f};
	return hit;
}

struct Object
{
	float roughness;
	float emission;
	Color color;

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

struct Mesh : public Object
{
	Vector position;
	// TODO: Rotation, Scale

	float radius_sqrd;

	int num_vertices;
	Vertex *vertices;

	virtual RayHit intersect(Vector global_origin, Vector ray)
	{
		Vector point; 
		Vector normal;
		Vector origin = global_origin - position;

		// If we're going in the wrong direction, ignore ray.
		if (dot(ray, position) < 0) return {};

		// If we're outside the bounding sphere, ignore ray.
		Vector closest_point = origin + ray * dot(ray, -origin);
		float dist_sqrd = length_sq(closest_point);
		if (dist_sqrd > radius_sqrd) return {};

		RayHit closest_hit = {};
		float closest_distance = 0;
		for (int i = 0; i < num_vertices; i += 3)
		{
			Vector a = vertices[i + 0].position;
			Vector b = vertices[i + 1].position;
			Vector c = vertices[i + 2].position;
			Vector normal = -vertices[i + 0].normal;
			float distance_from_origin = dot(a, normal) - dot(origin, normal);
			point = origin + ray * (distance_from_origin / dot(ray, normal));
			
			// is_point_in_triangle?
			{
				Vector right_vector;
				right_vector = cross(b - a, normal);
				if (dot((point - a), right_vector) < 0) continue;
				right_vector = cross(c - b, normal);
				if (dot((point - b), right_vector) < 0) continue;
				right_vector = cross(a - c, normal);
				if (dot((point - c), right_vector) < 0) continue;
				
				Vector global_point = point + position;
				RayHit hit = {global_point, normal, this};

				if (0 < distance_from_origin && (!closest_hit.object || distance_from_origin < closest_distance))
				{
					closest_distance = distance_from_origin;
					closest_hit = hit;
				}
			}
		}
		return closest_hit;
	}
};

Mesh make_mesh(Vertex *vertices, int num_vertices, Vector position)
{
	Mesh m;
	m.vertices = vertices;
	m.num_vertices = num_vertices;
	m.position = position;

	float distance_squared = length_sq(vertices[0].position);
	for (int i = 1; i < num_vertices; i++)
	{
		distance_squared = MAX(length_sq(vertices[i].position), distance_squared);
	}
	m.radius_sqrd = distance_squared;
	return m;
}



