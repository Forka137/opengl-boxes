#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader_s.h>
#include <stb_image.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum RotationAxis {
	X, Y, Z
};

class SpaceRotation {
public:
	SpaceRotation() {
		// Default constructor
		// x = 0, y = 1, z = 2
	};

	float GetX() {
		return _axisArray[0];
	}

	float GetY() {
		return _axisArray[1];
	}

	float GetZ() {
		return _axisArray[2];
	}

	void SetRotationAxis(RotationAxis axis) {
		_axis = axis;
	}

	void Rotate(float degrees) {
		if ((int)_axis < RotationAxis::X || (int)_axis > RotationAxis::Z) return;

		_axisArray[(int)_axis] += degrees;

		if (_axisArray[(int)_axis] >= 360.0f) {
			_axisArray[(int)_axis] = 0.0f;
		}
		// Si invertimos la rotación.
		else if (_axisArray[(int)_axis] < 0.0f) {
			_axisArray[(int)_axis] = 360.0f;
		}
	}

private:
	float _axisArray[3] = {0.0f, 0.0f, 0.0f};
	RotationAxis _axis = RotationAxis::Y;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void getRotationAxis(GLFWwindow* window);
void handleKeyboardMovement(GLFWwindow* window);
void showFpsOnConsole(float delta);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float BOX_BASE_ROTATION_SPEED = 100.0f;
const float CAM_BASE_ROTATION_SPEED = 100.0f;
const float CAM_BASE_SPEED = 3.0f;

unsigned int var_width = SCR_WIDTH;
unsigned int var_height = SCR_HEIGHT;

float mixValue = 0.2f;
float camYaw = 270.0f;
float camPitch = 0.0f;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
// CamFront es negativo, ya que hacia adelante es apuntar hacia Z negativo
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

SpaceRotation boxRotation;
unsigned int currentAxis = 1;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float oneSecond = 0.0f;

// Mouse cursor start position
float lastX = 400, lastY = 300;
const float sensitivity = 0.1f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MacOS, se puede hacer #ifdef.

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Cambiar tamaño del viewport cuando la ventana cambie de tamaño.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Vertex Input (para un cubo)
		// Vertex data: Positions (3D), texture coords (2D)
		float vertices[] = {
			//Positions      // //Texture Coords //
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 2.0f,  5.0f,-15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f,-12.3f),
			glm::vec3( 2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3( 1.3f, -2.0f, -2.5f),
			glm::vec3( 1.5f,  2.0f, -2.5f),
			glm::vec3(-2.5f, -0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		// Generating texture
		unsigned int texture1, texture2;
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// Filtering options
		// s, t, r = x, y, z
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load image 1
		int width, height, nrChannels;
		// color channels, las rutas no usan ../
		unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);

		if (data) {
			// Genera la textura, usando los valores entregados por la función stbi_load.
			// texture target, mipmap level, format, width, height, 0, loaded as RGB values and stored as chars (bytes), image data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			// Generar mipmaps
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture 1" << std::endl;
		}


		// Load image 2
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Filtering options
		// s, t, r = x, y, z
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Debido a que el sistema coordenado x,y es distinto para OpenGL y PNG.
		// Se debe invertir esta textura verticalmente.
		stbi_set_flip_vertically_on_load(true); 
		data = stbi_load("assets/frog.png", &width, &height, &nrChannels, 0);

		if (data) {
			// usamos GL_RGBA ya que la imagen es PNG				//		//
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture 2" << std::endl;
		}

		// Liberar memoria
		stbi_image_free(data);

		// Vertex array object, contiene información del vertex data + atributos (color, textura, etc).
		// "bind" could be seen as "select", to select a buffer.
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Vertex buffer object, necesita tener un ID único, se genera con glGenBuffers
		// Se usa la memoria de video (VRAM).
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Copies vertex data to the buffer, GL_STATIC_DRAW = the data is set only once and used many times.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Create vertex and fragment shader respectively
		Shader myShader("shaderfiles/06vshader.glsl", "shaderfiles/06fshader.glsl");

		// Tell OpenGL how to read the vertex data
		// Cada valor o punto esta guardado en un float de 4 bytes.
		// location en el VAO, vec3, type of value, normalize?, stride, offset
		// Position
		unsigned int stride = 5 * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinates
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Texture units
		// Indicar cual es el uniform sampler para cada textura en el shader.

		myShader.use(); // Se debe seleccionar el shader antes de seleccionar las texturas
		// glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0); // alternativa manual
		myShader.setInt("texture1", 0);
		myShader.setInt("texture2", 1);

		// Indicar en que posición esta cargada cada textura. (del total de 32 o 16).
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Show wireframe
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	// state-setting, especifica el color.

		unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(myShader.ID, "projection");

		glEnable(GL_DEPTH_TEST);

		// Mouse Setup
		// Hide mouse and lock it in the center.
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetCursorPosCallback(window, mouse_callback);


	// Render loop
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		showFpsOnConsole(deltaTime);

		// Función que detecta Input del teclado.
		processInput(window);

		// Rotación de los cubos 
		boxRotation.Rotate(-BOX_BASE_ROTATION_SPEED * deltaTime);

		// Rendering
		// Limpia el buffer con el color seleccionado y el z-depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
		
		// Triangulo
			myShader.use();
			// Actualizar el valor del mix
			myShader.setFloat("mixValue", mixValue);

			// Queremos hacer esto dentro del vertex shader
			// v_clip = m_projection * m_view * m_model * v_local

			// Ingresar la matriz view
			// Actualizar la rotación de la cámara.
			camFront.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
			camFront.y = sin(glm::radians(camPitch));
			camFront.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));
			camFront = glm::normalize(camFront); // según yo no es necesario

			glm::mat4 view;
			// CamPosition, CamTarget, CamUp
			view = glm::lookAt(camPos, camPos + camFront, camUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			
			// Ingresar matriz de proyección
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(45.0f), (float)var_width / (float)var_height, 0.1f, 100.0f);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);

			for (unsigned int i = 0; i < 10; i++) {
				// Actualizar rotación de los cubos
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);

				// La rotamos con nuestro input
				model = glm::rotate(model, glm::radians(boxRotation.GetX()), glm::vec3(1.0, 0.0, 0.0));
				model = glm::rotate(model, glm::radians(boxRotation.GetY()), glm::vec3(0.0, 1.0, 0.0));
				model = glm::rotate(model, glm::radians(boxRotation.GetZ()), glm::vec3(0.0, 0.0, 1.0));

				// Actualizar la matriz modelo en el vertex shader
				// uniform location, matrix amount, transpose?, matrix data
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				// Dibujar triángulos
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

		// Reemplaza el buffer actual de la ventana con el buffer de GLFW
		// Tiene que ver con el Double buffer. Se reemplaza el de adelante con el
		// de atrás.
		glfwSwapBuffers(window);

		// Revisar y realizar las últimas acciones de los eventos
		glfwPollEvents();
	}

	// Liberar memoria ocupada y devuelve los valores por defecto
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Actualiza los valores de ancho y alto para calcular la relación de aspecto
	var_width = width;
	var_height = height;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	// Opacity inputs
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.0005f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.0005f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
	getRotationAxis(window);
	handleKeyboardMovement(window);


	// Cam Rotation inputs
	/*if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camYaw += CAM_BASE_ROTATION_SPEED * deltaTime;
		if (camYaw >= 360.0f)
			camYaw = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camYaw -= CAM_BASE_ROTATION_SPEED * deltaTime;
		if (camYaw <= 0.0f)
			camYaw = 360.0f;
	}*/
}

void getRotationAxis(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		boxRotation.SetRotationAxis(RotationAxis::X);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		boxRotation.SetRotationAxis(RotationAxis::Y);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		boxRotation.SetRotationAxis(RotationAxis::Z);
	}
}

void handleKeyboardMovement(GLFWwindow* window) {
	float camSpeed = CAM_BASE_SPEED * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camPos += camSpeed * camFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camPos -= camSpeed * camFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// Producto cruz para obtener el vector camRight
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camPos += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Ypos is upside-down to the screen coordinates

	lastX = (float)xpos;
	lastY = (float)ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camYaw += xoffset;
	camPitch += yoffset;

	// Previene que el producto cruz entre camUp y camFront sea 0, por mirar verticalmente
	if (camPitch > 89.0f) {
		camPitch = 89.0f;
	}
	if (camPitch < -89.0f) {
		camPitch = -89.0f;
	}
}

void showFpsOnConsole(float delta) {
	if (oneSecond < 1.0) {
		oneSecond += delta;
	}
	else {
		std::cout << "FPS: " << 1.0 / delta << std::endl;
		oneSecond = 0.0;
	}
}