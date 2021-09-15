#include <lgl/Shader.h>

#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <glad/glad.h>

#include <lgl/Exception.h>

namespace lgl {

Shader::ShaderDeleter Shader::shaderDeleter = [](auto shader) {
    glDeleteShader(*shader);
    delete shader;
};

Shader::Shader(const std::string &pathname, int type) :
    shader(new unsigned int(glCreateShader(type)), shaderDeleter) {

    // Read shader source file
    auto fdDeleter = [pathname](int *fd){
        auto status = close(*fd);
        delete fd;
        if (status == -1) {
            throw BuildError("Failed to close() " + pathname + " - " + strerror(errno));
        }
    };
    std::unique_ptr<int, decltype(fdDeleter)> fd(
                new int(open(pathname.c_str(), O_RDONLY)),
                fdDeleter);
    if (*fd == -1) {
        throw BuildError("Failed to open() " + pathname + " - " + strerror(errno));
    }

    struct stat statbuf;
    if (fstat(*fd, &statbuf) == -1) {
        throw BuildError("Failed to fstat() " + pathname + " - " + strerror(errno));
    }

    std::unique_ptr<char[]> buffer(new char[statbuf.st_size]);
    if (read(*fd, buffer.get(), statbuf.st_size) != statbuf.st_size) {
        throw BuildError("Failed to read() from " + pathname + " - " + strerror(errno));
    }

    auto shaderCode = buffer.get();
    glShaderSource(*this->shader, 1, &shaderCode, NULL);
    glCompileShader(*this->shader);

    int status;
    glGetShaderiv(*this->shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        int logLength = 0;
        glGetShaderiv(*this->shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(*this->shader, logLength, nullptr, log.data());
        throw BuildError(std::string(log.begin(), log.end()));
    }
}

} // namespace lgl
