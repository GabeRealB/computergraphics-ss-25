#version 410 core

struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
};

struct Camera {
    vec3 position;
    vec3 near_plane_p0;
    vec3 near_plane_right;
    vec3 near_plane_up;
    vec2 pixel_size;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Plane {
    vec3 normal;
    float distance;
};

out vec3 color;

uniform uint num_triangles;
uniform Triangle triangles[100];
uniform Camera camera;

bool rayPlaneIntersection(Ray ray, Plane plane, out vec3 intersection, out float t) {
    float denom = dot(plane.normal, ray.direction);
    if (abs(denom) > 0.0001) {
        t = -(plane.distance + dot(plane.normal, ray.origin)) / denom;
        if (t >= 0) {
            intersection = ray.origin + ray.direction * t;
            return true;
        }
    }
    return false;
}

bool rayTriangleIntersection(Ray ray, Triangle triangle, out vec3 intersection, out float t) {
    vec3 edge0 = triangle.v1 - triangle.v0;
    vec3 edge1 = triangle.v2 - triangle.v0;
    vec3 triangle_normal = normalize(cross(edge0, edge1));
    Plane plane = Plane(triangle_normal, -dot(triangle_normal, triangle.v0));
    if (rayPlaneIntersection(ray, plane, intersection, t)) {
        vec3 v0 = triangle.v0 - ray.origin;
        vec3 v1 = triangle.v1 - ray.origin;
        vec3 v2 = triangle.v2 - ray.origin;

        vec3 n0 = normalize(cross(v1, v0));
        vec3 n1 = normalize(cross(v2, v1));
        vec3 n2 = normalize(cross(v0, v2));

        float d0 = -dot(ray.origin, n0);
        float d1 = -dot(ray.origin, n1);
        float d2 = -dot(ray.origin, n2);

        float c0 = dot(intersection, n0) + d0;
        float c1 = dot(intersection, n1) + d1;
        float c2 = dot(intersection, n2) + d2;

        if (c0 >= 0 && c1 >= 0 && c2 >= 0) {
            return true;
        }
    }

    return false;
}

void main()
{
    vec2 pixel_positions = gl_FragCoord.xy;

    vec3 x_offset = pixel_positions.x * camera.pixel_size.x * camera.near_plane_right;
    vec3 y_offset = pixel_positions.y * camera.pixel_size.y * camera.near_plane_up;
    vec3 pixel_world_space = camera.near_plane_p0 + x_offset + y_offset;

    Ray ray = Ray(camera.position, normalize(pixel_world_space - camera.position));

    float min_distance = 5.0;
    for (int i = 0; i < num_triangles; i++) {
        Triangle triangle = triangles[i];
        vec3 intersection;
        float t;
        if (rayTriangleIntersection(ray, triangle, intersection, t)) {
            if (t < min_distance) {
                min_distance = t;
            }
        }
    }

    color = mix(vec3(1.0), vec3(0.0), min(min_distance / 5.0, 1.0));
}
