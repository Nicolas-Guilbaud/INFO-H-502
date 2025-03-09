#include <glm/glm.hpp>
/**
 * Represents a light source
 */
class Light{
public:
    glm::vec3 position;
    glm::vec3 spectrum;
    Light(glm::vec3 position, glm::vec3 spectrum) : position(position), spectrum(spectrum){}
};
