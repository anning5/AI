////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Solid color shader
//
// shader which renders with a solid color

namespace octet {
  class ray_casting_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;
    GLuint samplerIndex_;
    GLuint camera_;

    // index for flat shader emissive color
  public:
    void init() {
      // this is the vertex shader. main() will be called for every vertex
      // of every triangle. The output is gl_Position which is used to generate
      // 2D triangles in the rasterizer.
      const char vertex_shader[] = SHADER_STR(
        attribute vec4 pos;
        attribute vec3 uv;
        uniform mat4 modelToProjection;
	varying vec3 uv_;
	varying vec3 pos_;
        void main() {
	gl_Position = modelToProjection * pos;
	uv_ = uv;
	pos_ = pos.xyz;
	}
      );

      // this is the fragment shader. It is called once for every pixel
      // in the rasterized triangles.
      const char fragment_shader[] = SHADER_STR(
	      varying vec3 uv_;
	      varying vec3 pos_;
	      uniform sampler3D sampler;
	      uniform vec3 camera;
	      void main() {
	      	int rate = 100;
		      vec3 dir = normalize(pos_ - camera);
		      vec3 stepVec = 1.f / rate * dir;
		      vec3 pos = pos_;
			  gl_FragColor = vec4(0, 0, 0, 0);
		      for(int i = 0; i < rate; i++)
		      {
			      pos += stepVec;
			      gl_FragColor += texture3D(sampler, pos);
			      if(pos.x > 1 || pos.y > 1 || pos.z > 1 || pos.x < 0 || pos.y < 0 || pos.z < 0)
			      {
				      break;
			      }
		      }
	      }
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      samplerIndex_ = glGetUniformLocation(program(), "sampler");
      camera_ = glGetUniformLocation(program(), "camera");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, GLuint sampler, vec3 &camera) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform1i(samplerIndex_, sampler);
      glUniform3fv(camera_, 1, (float *)&camera);
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}
