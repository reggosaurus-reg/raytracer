
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define CLAMP(min, max, v) (MAX(MIN(v, max), min))

struct Pixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

Pixel P(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return {r, g, b, a};
}

struct Color
{
	//TODO: Remove alpha
	union
	{
		struct
		{
			float r, g, b, a;
		};
		struct
		{
			float red, green, blue, alpha;
		};
	};


	Color operator* (float s) const;
	Color operator/ (float s) const;
	Color operator+= (Color p);
};

Color C(float r, float g, float b, float a)
{
	return {r, g, b, a};
}

Pixel to_pixel(Color color)
{
	return P(
			(uint8_t) CLAMP(0.0, 255.0, color.r * 255),
			(uint8_t) CLAMP(0.0, 255.0, color.g * 255),
			(uint8_t) CLAMP(0.0, 255.0, color.b * 255) ,
			(uint8_t) CLAMP(0.0, 255.0, color.a * 255) );
}

Color Color::operator* (float s) const
{
	return C(r * s, g * s, b * s, a * s);
}

Color Color::operator/ (float s) const
{
	return C(r / s, g / s, b / s, a / s);
}

Color Color::operator+= (Color p)
{
	r += p.r;
	g += p.g;
	b += p.b;
	a += p.a;
	return *this;
}

Color mix(Color a, Color b, float lerp)
{
	return C(
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

