#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <glm/glm.hpp>
#include <string>
#include <vector>

// Triangle structure to hold vertices, normals, UVs, and color
struct Triangle {
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;

    Triangle()
        : v1(0.0f)
        , v2(0.0f)
        , v3(0.0f)
    {
    }
};

// Function to load an OBJ file and extract triangles
inline bool load_obj(
    const std::filesystem::path& path,
    std::vector<Triangle>& out_triangles)
{
    // Temporary storage for OBJ data
    std::vector<unsigned int> vertexIndices;
    std::vector<glm::vec3> temp_vertices;

    // Open the OBJ file
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file!\n");
        return false;
    }

    // Read the file line by line
    int line = 0;
    while (1) {
        line++;
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) {
            fclose(file);
            break;
        }

        // Parse vertices
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "#") == 0) {
            continue;
        }
        // Parse faces
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3];
            int matches = fscanf(file, "%d %d %d\n",
                &vertexIndex[0],
                &vertexIndex[1],
                &vertexIndex[2]);
            if (matches != 3) {
                printf("File can't be read by our simple parser: ( Try exporting with other options) \n");
                // print line
                printf("Line: %d\n", line);
                return false;
            }
            // Store indices
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
        }
    }

    // Calculate min and max extents
    glm::vec3 min_extents(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max_extents(FLT_MIN, FLT_MIN, FLT_MIN);

    for (const auto& vertex : temp_vertices) {
        min_extents = glm::min(min_extents, vertex);
        max_extents = glm::max(max_extents, vertex);
    }

    // Calculate scale and translation to normalize the object
    glm::vec3 center = (max_extents + min_extents) / 2.0f;
    glm::vec3 size = max_extents - min_extents;
    float max_size = std::max(std::max(size.x, size.y), size.z);
    float scale = 2.0f / max_size; // Scale to fit in [-1, 1]


    // Process the data to create Triangle structures
    for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
        Triangle tri;

        // Vertex positions
        tri.v1 = (temp_vertices[vertexIndices[i] - 1] - center) * scale;
        tri.v2 = (temp_vertices[vertexIndices[i + 1] - 1] - center) * scale;
        tri.v3 = (temp_vertices[vertexIndices[i + 2] - 1] - center) * scale;

        out_triangles.push_back(tri);
    }

    return true;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // OBJLOADER_HPP
