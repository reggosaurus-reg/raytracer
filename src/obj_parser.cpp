#include <stdio.h>
#include <vector>
// TODO: Check if the reading is correct.
struct Vertex
{
	Vector position;
	Vector normal;
};

void read_until(const char *text, int *read_head, char c)
{
	while (text[*read_head] != c && text[*read_head])
		(*read_head)++;
}

void read_all_whitespace(const char *text, int *read_head)
{
	while (text[*read_head] == ' ' || text[*read_head] == '\t')
		(*read_head)++;
}

int read_int(const char *text, int *read_head)
{
	read_all_whitespace(text, read_head);
	
	int head = *read_head;
	int sign = 1;
	int integer_part = 0;

	if (text[head] == '-')
	{
		sign = -1;
		head++;
	}

	while (true)
	{
		char c = text[head];
		if ('0' <= c && c <= '9') {
			integer_part *= 10;
			integer_part += c - '0';
			head++;
			continue;
		}
		break;
	}
	*read_head = head;
	return sign * integer_part;
}

float read_float(const char *text, int *read_head)
{
	read_all_whitespace(text, read_head);

	int head = *read_head;
	bool read_dot = false;
	int sign = 1;
	float integer_part = 0;
	float decimal_part = 0;
	float decimals = 0;

	// TODO: Rewrite this with read_int.
	bool reading = true;
	while (reading)
	{
		char c = text[head++];
		switch (c) {
			case '-':
				sign = -1;
				break;
			case '.':
				read_dot = true;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					int n = c - '0';
					if (read_dot) {
						decimal_part *= 10;
						decimal_part += n;
						decimals++;
					} else {
						integer_part *= 10;
						integer_part += n;
					}
				}
				break;
			default:
				reading = false;
				break;
		}
	}
	*read_head = head;
	return sign * (integer_part + decimal_part * pow(10.0f, -decimals));

}

// TODO: List
#define List std::vector

List<Vertex> read_obj(const char *path)
{
	FILE *file = fopen(path, "r");
	size_t line_length = 0;
	char *line = 0;

	List<Vector> positions;
	List<Vector> normals;
	List<Vertex> vertices;

	if (!file) 
		return vertices;

	while (getline(&line, &line_length, file) != -1) 
	{
		int read_head = 0;
		if (line[0] == 'v' && line[1] == ' ')
		{
			read_head += 2;
			// Position
			Vector position;
			position.x = read_float(line, &read_head);
			position.y = read_float(line, &read_head);
			position.z = read_float(line, &read_head);
			positions.push_back(position);
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			read_head += 3;
			// Normal
			Vector normal;
			normal.x = read_float(line, &read_head);
			normal.y = read_float(line, &read_head);
			normal.z = read_float(line, &read_head);
			normals.push_back(normal);
		}
		else if (line[0] == 'f')
		{
			// Face
			read_head += 2;
			for (int i = 0; i < 3; i++) {
				int p = read_int(line, &read_head);
				read_head += 1;
				read_until(line, &read_head, '/');
				read_head += 1;
				int n = read_int(line, &read_head);
				vertices.push_back({positions[p - 1], normals[n - 1]});
			}
		}
	}
	free(line);
	assert(vertices.size() != 0);
	assert(vertices.size() % 3 == 0);
	return vertices;
}

