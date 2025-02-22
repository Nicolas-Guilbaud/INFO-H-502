#include <glm/glm.hpp>
/**
 * Represents a light source
 */
class Light{
public:
    glm::vec3 position;
    Light(glm::vec3 position) : position(position){}
};