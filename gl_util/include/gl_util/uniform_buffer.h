#pragma once

namespace gl {

///
/// \brief Wrapper class for uniform buffer objects.
///
class UniformBuffer {
public:
    ///
    /// \brief UniformBuffer Creates and allocates a uniform buffer object of
    ///                      specified size and links it against a generated
    ///                      binding point.
    /// \param size_bytes Number of bytes to allocate in the uniform buffer
    ///                   object.
    ///
    explicit UniformBuffer(unsigned int size_bytes);

    ///
    /// Releases its binding point to allow it to be linked with another
    /// UniformBuffer object and deletes the uniform buffer object on the GPU.
    ///
    ~UniformBuffer();

    ///
    /// \brief getBindingPoint Returns the binding point this object is linked
    ///                        against for use in linking uniform blocks against.
    /// \return The binding point.
    ///
    unsigned int getBindingPoint() const;

    ///
    /// \brief bufferSubData Sets the data within this uniform buffer object.
    /// \param offset_bytes Offset in bytes into the uniform buffer object's data
    ///                     store where the data replacement will begin.
    /// \param size_bytes Size in bytes of data being replaced.
    /// \param data Pointer to data to copy into this uniform buffer object.
    /// \return This object reference for convenient chaining of function calls.
    ///
    UniformBuffer& bufferSubData(unsigned int offset_bytes,
                                 unsigned int size_bytes,
                                 const void *data);

private:
    unsigned int ubo;
    unsigned int bindingPoint;
};

inline unsigned int UniformBuffer::getBindingPoint() const {return this->bindingPoint;}

} // namespace gl
