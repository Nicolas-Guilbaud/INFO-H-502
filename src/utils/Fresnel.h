#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

class Fresnel {
public:
    // Map of metal names to their Fresnel reflectance values
    std::unordered_map<std::string, glm::vec3> fresnelData;

    Fresnel(const std::string& filename) {
        loadFresnelData(filename);
    }

    // Function to get Fresnel value for a given material
    glm::vec3 getFresnelValue(const std::string& material) {
        // Map of metal names to their Fresnel reflectance values
        if (fresnelData.find(material) != fresnelData.end()) {
            return fresnelData[material];
        }
        return glm::vec3(0.04); // Default for non-metals
    }

    const std::vector<std::string> getMaterialList() {
        std::vector<std::string> list; 

        for (const auto& pair : fresnelData) {
            list.push_back(pair.first);
        }
        return list;
    }

private:
    void loadFresnelData(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error: Cannot open Fresnel data file!" << std::endl;
            return;
        }
        else {
            std::cout << "File opened" << std::endl;
        }
        std::string line, name;
        float r, g, b;

        while (std::getline(file, line)) {
            if (line[0] == '#' || line.empty()) continue; // Ignore comments

            std::istringstream ss(line);
            ss >> name >> r >> g >> b;
            fresnelData[name] = glm::vec3( r, g, b );
        }
    }
};

