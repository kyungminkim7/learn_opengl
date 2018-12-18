#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <gl_util/shader_program.h>

namespace gl {

///
/// \brief Contains the pose data.
///
class Model
{
public:
    glm::mat4 getModelMatrix() const;

    ///
    /// \brief getNormalMatrix Returns the normal matrix.
    ///
    /// The returned normal matrix can be used to correct vertex normal vectors distorted through
    /// non-uniform scaling of the model.
    ///
    /// The recommended use of this function is to use this to set a uniform value in an OpenGL
    /// vertex shader and then multiply by the vertex normal.
    ///
    /// Ex) correctedNormal = normalMatrix * vertexNormal;
    ///
    /// \return The normal matrix of the model.
    ///
    glm::mat3 getNormalMatrix() const;

    Model& setPosition(const glm::vec3 &position);
    glm::vec3 getPosition() const;

    Model& setOrientation(const glm::mat3 &orientation);
    Model& setOrientation(const glm::vec3 &orientationX,
                          const glm::vec3 &orientationY,
                          const glm::vec3 &orientationZ);
    glm::mat3 getOrientation() const;
    glm::vec3 getOrientationX() const;
    glm::vec3 getOrientationY() const;
    glm::vec3 getOrientationZ() const;

    ///
    /// \brief rotate Rotates the model about an axis in the world coordinate frame.
    /// \param angle_rad Angle to rotate model by (rad).
    /// \param axis Axis (in world coordinate frame) to rotate model about.
    /// \return The model to allow chaining of multiple rotation/translation calls.
    ///
    Model& rotate(float angle_rad, const glm::vec3 &axis);

    ///
    /// \brief translate Translates the model in the world coordinate frame.
    /// \param translation Amount to translate the model by in the world coordinate frame.
    /// \return The model to allow chaining of multiple rotation/translation calls.
    ///
    Model& translate(const glm::vec3 &translation);

    ///
    /// \brief translateInBodyFrame Translates the model in the local coordinate frame.
    /// \param translation Amount to translate the model by in the local coordinate frame.
    /// \return The model to allow chaining of multiple rotation/translation calls.
    ///
    Model& translateInLocalFrame(const glm::vec3 &translation);

    Model& setScale(const glm::vec3 &scale);

    ///
    /// \brief render Sets the uniform model and normal matrices on the shader.
    /// \param shader
    ///
    void render(ShaderProgram *shader);

private:
    glm::vec3 scale {1.0f};

    glm::vec3 position {0.0f};
    glm::mat3 orientation {1.0f};

    mutable glm::mat3 normalMatrix {1.0f};
    mutable bool normalMatrixIsValid = true;
};

inline glm::vec3 Model::getPosition() const {return this->position;}
inline glm::mat3 Model::getOrientation() const {return this->orientation;}
inline glm::vec3 Model::getOrientationX() const {return this->orientation[0];}
inline glm::vec3 Model::getOrientationY() const {return this->orientation[1];}
inline glm::vec3 Model::getOrientationZ() const {return this->orientation[2];}

} // namespace gl
