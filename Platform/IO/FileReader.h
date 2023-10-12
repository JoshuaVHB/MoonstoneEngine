#pragma once

#include <tuple>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "../../Graphics/World/Mesh.h"

namespace fs = std::filesystem;

static void skipStreamText(std::istream& stream, const char* text)
{
    char c;
    while (*text) {
        if ((c = stream.get()) != *text) {
            std::cerr << "Expected " << *text << " but got " << c;
            throw std::runtime_error("Unexpected charactor got in stream");
        }
        text++;
    }
}

static std::pair<std::vector<Vertex>, std::vector<uint16_t>> readMeshFromObj(const fs::path& path) 
{

	std::ifstream modelFile{ path };

    /*
    if (!modelFile.is_open())
        WARNING("The model file " << path << " has not been found.")
        return { {},{} };
    */
	constexpr size_t bufSize = 100;
	char lineBuffer[bufSize];

	std::vector<XMVECTOR> positions;
	std::vector<XMVECTOR> normals;

    std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

     std::vector<std::tuple<int, int, int>> cachedVertices;
    unsigned int previousCachedVertices = 0;


    int l = 0;
    while (modelFile.good()) {
        l++;
        modelFile.getline(lineBuffer, bufSize);
        if (lineBuffer[0] == '#')
            continue;
        int space = 0;
        while (lineBuffer[space] != '\0' && lineBuffer[space] != ' ')
            space++;
        if (lineBuffer[space] == '\0')
            continue;
        std::stringstream ss;
        ss.str(lineBuffer + space + 1);

        float f1, f2, f3;
        int i1 = 0, i2 = 0, i3 = 0;
        if (strstr(lineBuffer, "v ") == lineBuffer) { // vertex
            ss >> f1 >> f2 >> f3;
            positions.push_back({ f1, f2, f3 });

        }
        else if (strstr(lineBuffer, "vn ") == lineBuffer) { // normal
            ss >> f1 >> f2 >> f3;
            normals.push_back({ f1, f2, f3 });

        }
        else if (strstr(lineBuffer, "f ") == lineBuffer) { // face
            for (size_t i = 0; i < 3; i++) {
                ss >> i1;                      // read vertex
                skipStreamText(ss, "/");
                if (ss.peek() != '/') ss >> i2; // read optional uv
                skipStreamText(ss, "/");
                if (ss.peek() != '/') ss >> i3; // read optional uv
                ss >> i3;                      // read normal
                std::tuple<int, int, int> cacheKey{ i1, i2, i3 };
                auto inCacheIndex = std::find(cachedVertices.begin(), cachedVertices.end(), cacheKey);
                if (inCacheIndex == cachedVertices.end()) {
                    vertices.emplace_back(Vertex{ positions[i1 - 1], normals[i3 - 1] });
                    indices.push_back(previousCachedVertices + (unsigned int)cachedVertices.size());
                    cachedVertices.push_back(cacheKey);
                }
                else {
                    indices.push_back(previousCachedVertices + (unsigned int)(inCacheIndex - cachedVertices.begin()));
                }
            }
        }
    }
    return { vertices, indices };
        



}