
#define MAX(a, b) (a > b ? a : b)

struct Pixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;

	Pixel operator* (float s) const;
};

Pixel P(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return {r, g, b, a};
}

Pixel Pixel::operator* (float s) const
{
	return P(red * s, green * s, blue * s, alpha);
}

Pixel mix(Pixel a, Pixel b, float lerp)
{
	return P(
		a.red * lerp + b.red * (1.0f - lerp),
		a.green * lerp + b.green * (1.0f - lerp),
		a.blue * lerp + b.blue * (1.0f - lerp),
		a.alpha * lerp + b.alpha * (1.0f - lerp));
}


struct Vector
{
	float x, y, z;

	Vector operator* (float s) const
	{
		return {x * s, y * s, z * s};
	}

	Vector operator- () const
	{
		return {-x, -y, -z};
	}

	Vector operator- (Vector other) const
	{
		return {x - other.x, y - other.y, z - other.z};
	}

	Vector operator+ (Vector other) const
	{
		return {x + other.x, y + other.y, z + other.z};
	}
};

inline
Vector V3(float x, float y, float z)
{
	return {x, y, z};
}

float dot(Vector a, Vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector reflect(Vector v, Vector n)
{
	// Assumes N is normalized.
	return v - n * 2 * dot(v, n);
}

float length_sq(Vector a)
{
	return dot(a, a);
}

float length(Vector a)
{
	return sqrt(length_sq(a));
}

Vector normalize(Vector a)
{
	return a * (1.0f / length(a));
}

