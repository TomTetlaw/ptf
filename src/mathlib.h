#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#ifndef M_PI_F
#define M_PI_F ((float)(M_PI))
#endif

inline float deg2rad(float x) {
	return x * (M_PI_F / 180.0f);
}

inline float rad2deg(float x) {
	return x * (180.0f / M_PI_F);
}

inline float approach(float current, float goal, float dt) {
	float dist = goal - current;
	if (dist > dt) {
		return current + dt;
	}
	else if (dist < -dt) {
		return current - dt;
	}
	else {
		return goal;
	}
}

inline bool within_tolerance(float x, float y, float tolerance) {
	if (x > y + tolerance) {
		return false;
	}
	else if (x < y - tolerance) {
		return false;
	}
	else {
		return true;
	}
}

template<typename T>
inline T clamp(T x, T min_value, T max_value) {
	if (x < min_value) {
		return min_value;
	}
	else if (x > max_value) {
		return max_value;
	}
	else {
		return x;
	}
}

inline float map_range(float x, float min1, float max1, float min2, float max2) {
	return (max2 - min2) * (x / (max1 - min1));
}

inline float random_float(float min_value, float max_value) {
	return min_value + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_value - min_value)));
}

#define V4PARMS(v) v.x,v.y,v.z,v.w
struct Vector4 {
	float x = 0, y = 0, z = 0, w = 0;

	Vector4() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float distance_to(Vector4 &other) {
		Vector4 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	Vector4 operator +(Vector4 &other) {
		return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
	}
	Vector4 operator -(Vector4 &other) {
		return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
	}
	Vector4 operator *(Vector4 &other) {
		return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
	}
	Vector4 operator +(float other) {
		return Vector4(x + other, y + other, z + other, w + other);
	}
	Vector4 operator -(float other) {
		return Vector4(x - other, y - other, z - other, w - other);
	}
	Vector4 operator *(float other) {
		return Vector4(x * other, y * other, z * other, w * other);
	}
};

#define V3PARMS(v) v.x,v.y,v.z
struct Vector3 {
	float x = 0, y = 0, z = 0;

	Vector3() {
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float distance_to(Vector3 &other) {
		Vector3 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y + z*z);
	}

	Vector3 operator +(Vector3 &other) {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}
	Vector3 operator -(Vector3 &other) {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	Vector3 operator *(Vector3 &other) {
		return Vector3(x * other.x, y * other.y, z * other.z);
	}
	Vector3 operator +(float other) {
		return Vector3(x + other, y + other, z + other);
	}
	Vector3 operator -(float other) {
		return Vector3(x - other, y - other, z - other);
	}
	Vector3 operator *(float other) {
		return Vector3(x * other, y * other, z * other);
	}
};

#define V2PARMS(v) v.x, v.y
struct Vector2 {
	float x = 0, y = 0;

	Vector2() {
		x = 0;
		y = 0;
	}

	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	float distance_to(Vector2 &other) {
		Vector2 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y);
	}

	float dot(Vector2 &other) {
		return x*other.x + y*other.y;
	}

	float determinant(Vector2 &other) {
		return x*other.y - y*other.x;
	}

	Vector2 normalize() {
		float len = length();
		return Vector2(x / len, y / len);
	}

	float angle_to(Vector2 &other) {
		Vector2 up, right, dir;
		float dot_product, side, angle;

		up = normalize();
		right = Vector2(-up.y, up.x);
		dir = other.normalize();
		dot_product = up.dot(dir);
		side = right.dot(dir);
		angle = acosf(dot_product);

		if (side < 0.0f) {
			angle *= -1.0f;
		}

		return angle;
	}

	Vector2 operator +(Vector2 &other) {
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator -(Vector2 &other) {
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 operator *(Vector2 &other) {
		return Vector2(x * other.x, y * other.y);
	}
	Vector2 operator +(float other) {
		return Vector2(x + other, y + other);
	}
	Vector2 operator -(float other) {
		return Vector2(x - other, y - other);
	}
	Vector2 operator *(float other) {
		return Vector2(x * other, y * other);
	}
};

inline Vector2 approach(Vector2 current, Vector2 goal, float dt) {
	return Vector2(approach(current.x, goal.x, dt), approach(current.y, goal.y, dt));
}

struct Vector2i {
	int x = 0;
	int y = 0;

	Vector2i() {
		x = 0;
		y = 0;
	}
	
	Vector2i(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Vector2i operator+(Vector2i &other) {
		return Vector2i(x + other.x, y + other.y);
	}

	bool operator==(Vector2i &other) {
		return x == other.x && y == other.y;
	}

	bool operator !=(Vector2i &other) {
		return !(*this == other);
	}
};

inline Vector2 v2_to_v2i(Vector2i v, float scale = 1.0f) {
	return Vector2(v.x * scale, v.y * scale);
}

struct Box {
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};

struct AABB {
	float min_x = 0.0f;
	float min_y = 0.0f;
	float max_x = 0.0f;
	float max_y = 0.0f;
};

inline Box aabb_to_box(AABB aabb) {
	Box out;
	out.x = aabb.min_x;
	out.y = aabb.min_y;
	out.width = aabb.max_x - aabb.min_x;
	out.height = aabb.max_y - aabb.min_y;
	return out;
}

inline AABB box_to_aabb(Box box) {
	AABB out;
	out.min_x = box.x;
	out.min_y = box.y;
	out.max_x = box.x + box.width;
	out.max_y = box.y + box.height;
	return out;
}

enum AABB_Inside_Direction {
	AABB_OUTSIDE,
	AABB_LEFT, 
	AABB_RIGHT,
	AABB_TOP, 
	AABB_BOTTOM,
};

inline bool aabb_inside(AABB a, AABB b) {
	if(a.max_x > b.min_x && a.min_x < b.max_x &&
		a.max_y > b.min_y && a.min_y < b.max_y) {
		return true;
	} else {
		return false;
	}
}

#endif
