#include "agt_glsl_shader.h"

static std::string glslVersion = "#version 330 core";

static std::map<std::string, agt3d::Shader> cachedShaders;

namespace agt3d
{

void log(const std::string& msg) { std::cerr << msg << "\n"; }

void setGlslVersion(const std::string& version) noexcept
{
  glslVersion = version;
}

std::string getGlslVersion() noexcept { return glslVersion; }

std::optional<std::string> validateShader(GLuint shaderIndex)
{
  GLint success = GL_FALSE;
  constexpr auto logSize = 4096;
  char infoLog[logSize]{'\0'};

  glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &success);
  if (success) {
    return std::nullopt;
  }
  glGetShaderInfoLog(shaderIndex, 4096, NULL, infoLog);
  std::cerr << infoLog << std::endl;
  std::stringstream ss;
  ss << "Shader Error: " << infoLog;
  return ss.str();
}

std::optional<std::string> validateProgram(GLuint programIndex)
{
  GLint success = GL_FALSE;
  constexpr auto logSize = 4096;
  char infoLog[logSize]{'\0'};

  glGetProgramiv(programIndex, GL_LINK_STATUS, &success);
  if (success) {
    return std::nullopt;
  }
  glGetProgramInfoLog(programIndex, logSize, NULL, infoLog);
  std::stringstream ss;
  ss << "Program Error: " << infoLog;
  return ss.str();
}

std::optional<std::string> loadSourceFromFile(const std::string& path) noexcept
{
  std::fstream file(path, std::ios::in);
  if (!file.is_open()) {
    return std::nullopt;
  }
  std::stringstream buf;
  buf << file.rdbuf();
  return buf.str();
}

std::optional<agt3d::Shader> loadGlslShaderFromFile(
  const std::string& path) noexcept
{
  auto source = loadSourceFromFile(path);
  if (source == std::nullopt) {
    log(path + ": failed to load");
    return std::nullopt;
  }

  agt3d::Shader shader;
  shader.glslShaderSource = *source;
  return shader;
}

std::map<std::string, GLuint> collectAttributes(GLuint program) noexcept
{
  GLint count;
  GLint size;
  GLenum type;
  const GLsizei bufSize = 256;
  GLsizei length;
  GLchar name[bufSize] = {'\0'};

  std::map<std::string, GLuint> attributes;

  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
  std::cout << "Active Attributes: " << count << "\n";

  for (GLint i = 0; i < count; i++) {
    glGetActiveAttrib(program, (GLuint)i, bufSize, &length, &size, &type, name);
    auto loc = glGetAttribLocation(program, name);
    std::cout << "Attribute #" << i << " Type: " << type << " Name: " << name
              << " Loc: " << loc << " Size: " << size << " Length: " << length
              << "\n";
    attributes[name] = loc;
  }

  return attributes;
}

std::map<std::string, GLuint> collectUniforms(GLuint program) noexcept
{
  GLint count;
  GLint size;
  GLenum type;
  const GLsizei bufSize = 256;
  GLsizei length;
  GLchar name[bufSize] = {'\0'};

  std::map<std::string, GLuint> uniforms;

  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
  std::cout << "Active Uniforms: " << count << "\n";

  for (GLint i = 0; i < count; i++) {
    glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type,
                       name);
    auto loc = glGetUniformLocation(program, name);
    uniforms[name] = loc;
    std::cout << "Uniform #" << i << " Type: " << type << " Name: " << name
              << " Loc: " << loc << " Size: " << size << " Length: " << length
              << "\n";
  }

  return uniforms;
}

std::optional<Shader> compileShader(const std::string& path) noexcept
{
  auto source = loadGlslShaderFromFile(path);
  if (!source) {
    return std::nullopt;
  }

  const GLchar* vertexSources[] = {getGlslVersion().c_str(), "\n",
                                   "#define VERTEX\n",
                                   source->glslShaderSource.c_str()};
  const GLchar* fragmentSources[] = {getGlslVersion().c_str(), "\n",
                                     "#define FRAGMENT\n",
                                     source->glslShaderSource.c_str()};

  auto vert = glCreateShader(GL_VERTEX_SHADER);
  auto frag = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vert, sizeof(vertexSources) / sizeof(*vertexSources),
                 vertexSources, NULL);
  glShaderSource(frag, sizeof(fragmentSources) / sizeof(*fragmentSources),
                 fragmentSources, NULL);

  glCompileShader(vert);
  if (validateShader(vert)) {
    return {};
  }
  glCompileShader(frag);
  if (validateShader(frag)) {
    return {};
  }

  auto prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  if (validateProgram(prog)) {
    return {};
  }

  // cleanup - this will delete the shaders once the program is not used any
  // more
  glDeleteShader(vert);
  glDeleteShader(frag);
#ifdef VERBOSE
  std::cout << "glsl compiled: " << prog << "\n";
#endif
  Shader shader;
  shader.program = prog;
  shader.glslShaderPath = path;
  shader.compiled = true;

  shader.attributes = collectAttributes(shader.program);
  shader.uniforms = collectUniforms(shader.program);

  return shader;
}

void unloadShader(Shader& shader) noexcept
{
  glDeleteProgram(shader.program);
  // todo
  log("### not implemented!");
  abort();
}

void cacheShader(const std::string& id, const Shader& shader) noexcept
{
  cachedShaders[id] = shader;
}

void unloadShader(std::string& id) noexcept
{
  if (!cachedShaders.contains(id)) {
    return;
  }
  unloadShader(cachedShaders[id]);
}

std::optional<const Shader*> getShader(const std::string& id) noexcept
{
  if (!cachedShaders.contains(id)) {
    return std::nullopt;
  }
  return &(cachedShaders[id]);
}

void useShader(const Shader& shader) noexcept { glUseProgram(shader.program); }
}  // namespace agt3d
