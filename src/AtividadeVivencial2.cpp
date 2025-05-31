#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


float playerX = 0.0f, playerY = 0.0f;
float speed = 0.0003f;
float speedY = 0.01f;
float backgroundLayerOffsets[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float repeatFactor[4] = {0.2f, 0.5f, 1.0f, 2.0f}; 

GLuint shaderID;
GLuint camadasTexturas[4];
GLuint personagemTextura;


const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;

out vec2 tex_coord;

uniform vec2 offset;
uniform float tex_repeat;
uniform vec2 player_offset; 

void main()
{
	tex_coord = vec2(texc.s + offset.x * tex_repeat, 1.0 - texc.t);
    gl_Position = vec4(position.xy + player_offset, position.z, 1.0); 
}

)";


const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec2 tex_coord;
 out vec4 color;

 uniform sampler2D tex;

 void main()
 {
	color = texture(tex, tex_coord);
 }
)";


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
		else if (key == GLFW_KEY_W)
			playerY += speedY;
		else if (key == GLFW_KEY_S)
			playerY -= speedY;
		else if (key == GLFW_KEY_A)
			playerX -= speed;
		else if (key == GLFW_KEY_D)
			playerX += speed;

		
		backgroundLayerOffsets[0] = playerX * 0.2f;
		backgroundLayerOffsets[1] = playerX * 0.5f;
		backgroundLayerOffsets[2] = playerX * 1.0f;
		backgroundLayerOffsets[3] = playerX * 2.0f;
	}
}

GLuint loadTexture(const char *filePath)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, STBI_rgb_alpha); 

    if (!data)
    {
        std::cout << "Erro ao carregar imagem: " << filePath << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); 
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

int main()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor *primary = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(primary);
	GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Gremio Run - Pedro Teixeira Alves, Felipe Bergamo, Diuliano Vargas da Silveira", NULL, NULL);

	if (!window)
	{
		std::cout << "Erro ao criar janela GLFW\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Erro ao carregar GLAD\n";
		return -1;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetKeyCallback(window, key_callback);
	glViewport(0, 0, mode->width, mode->height);

	
	GLfloat fundoVertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	};

	GLuint VBO_fundo, VAO_fundo;
	glGenVertexArrays(1, &VAO_fundo);
	glGenBuffers(1, &VBO_fundo);
	glBindVertexArray(VAO_fundo);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_fundo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fundoVertices), fundoVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	GLfloat personagemVertices[] = {
		-0.9f, -0.9f, 0.0f, 0.0f, 0.0f,
		-0.7f, -0.9f, 0.0f, 1.0f, 0.0f,
		-0.9f, -0.7f, 0.0f, 0.0f, 1.0f,
		-0.7f, -0.7f, 0.0f, 1.0f, 1.0f,
	};

	GLuint VBO_personagem, VAO_personagem;
	glGenVertexArrays(1, &VAO_personagem);
	glGenBuffers(1, &VBO_personagem);
	glBindVertexArray(VAO_personagem);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_personagem);
	glBufferData(GL_ARRAY_BUFFER, sizeof(personagemVertices), personagemVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderID);

	
	camadasTexturas[0] = loadTexture("../assets/backgrounds/sky.png");
	camadasTexturas[1] = loadTexture("../assets/backgrounds/rocks.png");
	camadasTexturas[2] = loadTexture("../assets/backgrounds/clouds_1.png");
	camadasTexturas[3] = loadTexture("../assets/backgrounds/ground_3.png");
	personagemTextura = loadTexture("../assets/sprites/Vampirinho.png");

	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderID);

		
		glBindVertexArray(VAO_fundo);

		GLint repeatLoc = glGetUniformLocation(shaderID, "tex_repeat");

		for (int i = 0; i < 4; i++)
		{
			glBindTexture(GL_TEXTURE_2D, camadasTexturas[i]);
			glUniform2f(glGetUniformLocation(shaderID, "offset"), backgroundLayerOffsets[i], 0.0f);
			glUniform2f(glGetUniformLocation(shaderID, "player_offset"), 0.0f, 0.0f); 

			
			float repeatFactor = 1.0f + i * 2.5f;  
			glUniform1f(repeatLoc, repeatFactor);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		
		glBindVertexArray(VAO_personagem);
		glBindTexture(GL_TEXTURE_2D, personagemTextura);
		glUniform1f(repeatLoc, 0.0f); 
		glUniform2f(glGetUniformLocation(shaderID, "offset"), 0.0f, 0.0f); 

		
		glUniform2f(glGetUniformLocation(shaderID, "player_offset"), playerX, playerY);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glPopMatrix(); 

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	glDeleteVertexArrays(1, &VAO_fundo);
	glDeleteBuffers(1, &VBO_fundo);
	glDeleteVertexArrays(1, &VAO_personagem);
	glDeleteBuffers(1, &VBO_personagem);
	glfwTerminate();
	return 0;
}
