#pragma once
#include "agt_stdafx.h"

namespace agt3d
{

class ObjectInstance;

void checkOpenGLErrors() noexcept;

std::vector<std::string> getFilesInFolder(const std::string& path) noexcept;

/**
 * @brief TODO: Please relocate me to the agt3d core project when done.
*/
  struct ray {
    glm::vec3 origin;
    glm::vec3 direction;
  };

  struct plane {
    glm::vec3 origin = { 0,0,0 };
    glm::vec3 normal = { 0,1,0 };

    plane() {}
    plane(glm::vec3 o, glm::vec3 n) : origin(o), normal(n) {}

    /**
     * @brief Projects a 3d point onto a 3d plane. Borrowed from https://vorg.github.io/pex/docs/pex-geom/Plane.html.
     * Updated: https://stackoverflow.com/questions/9605556/how-to-project-a-point-onto-a-plane-in-3d
     * @param p 3d point to project.
     * @return projected 3d point.
    */
    inline glm::vec3 projectPoint(const glm::vec3& p) const
    {
      auto d = p - origin;
      auto dist = glm::dot(d, normal);
      auto projected = p - dist * normal;
      return projected;
    }

    inline float distance(const glm::vec3& p) const
    {
      auto d = p - origin;
      auto dist = glm::dot(d, normal);
      return dist;
    }

    bool rayIntersect(const glm::vec3& rayP, const glm::vec3& rayDir, float& distance) const
    {
      return glm::intersectRayPlane(rayP, rayDir, origin, normal, distance);
    }
  };

glm::vec3 viewToWorldSpaceCoordTransform(float mouse_x, float mouse_y,
                                         float windowWidth, float windowHeight,
                                         glm::mat4 viewProjection) noexcept;

glm::vec3 raycast(float mx, float my, float windowWidth, float windowHeight,
                  const glm::mat4& view, const glm::mat4& proj) noexcept;
void dumpToFile(std::string filename, uint8_t* data, size_t len) noexcept;
void linearRegression(std::vector<glm::vec2> points, float& b,
                      float& m) noexcept;

/**
 * Get a normalized vector from the center of the virtual ball O to a
 * point P on the virtual ball surface, such that P is aligned on
 * screen's (X,Y) coordinates.  If (X,Y) is too far away from the
 * sphere, return the nearest point on the virtual ball surface.
 */
glm::vec3 get_arcball_vector(int x, int y, int screen_width,
                             int screen_height) noexcept;
glm::vec3 convertToSpherical(glm::vec3& vec) noexcept;
glm::vec3 convertToCartesian(glm::vec3 sp) noexcept;
template <typename T>
T lerp(T a, T b, T f)
{
  return a + f * (b - a);
}

uint64_t getSystemTimeUsec() noexcept;
uint64_t getSystemTimeMsec() noexcept;
std::string time_in_HH_MM_SS_MMM(const char* format) noexcept;
std::string getFilenameWithExtension(std::string fullpath) noexcept;
std::string getPath(std::string fullpath) noexcept;
agt3d::ObjectInstance* oiFactory(std::string&& name) noexcept;
agt3d::ObjectInstance* oiFactoryPlaneXZ(std::string&& name) noexcept;
agt3d::ObjectInstance* oiFactoryOverlay(std::string&& name) noexcept;

}  // namespace agt3d
