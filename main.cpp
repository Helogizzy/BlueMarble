#include<iostream>
#include<cassert>
#include<array>
#include<fstream>
#include<vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/ext.hpp>
#include<glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width = 800;
int height = 600;

std::string ReadFile(const char* FilePath) {
	std::string FileContents;
	if (std::ifstream FileStream{ FilePath, std::ios::in }) {
		FileContents.assign((std::istreambuf_iterator<char>(FileStream)), std::istreambuf_iterator<char>());
	}
	return FileContents;
}

void CheckShader(GLuint ShaderID) {
	//ShaderID tem que ser um identificador de um shader já compilado
	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE) {
		//obtém o tamanho do log
		GLint InfoLogLenght = 0;
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLenght);

		std::string ShaderInfoLog(InfoLogLenght, '\0');
		glGetShaderInfoLog(ShaderID, InfoLogLenght, nullptr, &ShaderInfoLog[0]);

		if (InfoLogLenght > 0) {
			std::cout << "Erro no shader" << std::endl;
			std::cout << ShaderInfoLog << std::endl;

			assert(false);
		}
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile) {
	//cria os identificadores
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSource = ReadFile(FragmentShaderFile);

	assert(!VertexShaderSource.empty());
	assert(!FragmentShaderSource.empty());

	std::cout << "Compilando " << VertexShaderFile << std::endl;
	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();
	glShaderSource(VertexShaderID, 1, &VertexShaderSourcePtr, nullptr);
	glCompileShader(VertexShaderID);
	CheckShader(VertexShaderID);

	std::cout << "Compilando " << FragmentShaderFile << std::endl;
	const char* FragmentShaderSourcePtr = FragmentShaderSource.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentShaderSourcePtr, nullptr);
	glCompileShader(FragmentShaderID);
	CheckShader(FragmentShaderID);

	std::cout << "Linkando o programa" << std::endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	//verifica a linkagem
	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE) {
		//pega o log para saber qual é o problema
		GLint InfoLogLenght = 0;
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLenght);

		if (InfoLogLenght > 0) {
			std::string ProgramInfoLog(InfoLogLenght, '\0');
			glGetProgramInfoLog(ProgramID, InfoLogLenght, nullptr, &ProgramInfoLog[0]);
			std::cout << "Erro ao linkar o programa" << std::endl;
			std::cout << ProgramInfoLog << std::endl;

			assert(false);
		}
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint LoadTexture(const char* TextureFile) {
	std::cout << "Carregando Textura" << TextureFile << std::endl;
	stbi_set_flip_vertically_on_load(true);

	int TextureWidth = 0, TextureHeight = 0, NumberOfComponents = 0;
	unsigned char* TextureData = stbi_load(TextureFile, &TextureWidth, &TextureHeight, &NumberOfComponents, 3);
	assert(TextureData);

	//gera o identificador de textura
	GLuint TextureID;
	glGenTextures(1, &TextureID);

	//habilita a textura para ser modifcada
	glBindTexture(GL_TEXTURE_2D, TextureID);

	//copia a textura para a memória de vídeo (GPU)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureWidth, TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);

	//filtros de magnificação e minificação
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// configuração do texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//gera o mipmap a partir da textura
	glGenerateMipmap(GL_TEXTURE_2D);

	//desliga a textura pois la já foi copiada para a gpu
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(TextureData);

	return TextureID;
}

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 UV; //cordenada de textura do vértice
};

struct DirectionalLight {
	glm::vec3 Direction;
	GLfloat Intensity;
};

GLuint LoadGeometry() {
	//quadrado em coordenadas normalizadas
	std::array<Vertex, 6 > quad = {
		Vertex{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f,0.0f,0.0f}, glm::vec2{0.0f, 0.0f}}, //vértice canto inferior esquerdo
		Vertex{glm::vec3{1.0f, -1.0f, 0.0f}, glm::vec3{0.0f,0.0f,1.0f}, glm::vec3{0.0f,1.0f,0.0f}, glm::vec2{1.0f, 0.0f}}, //vértice canto inferior direito
		Vertex{glm::vec3{1.0f, 1.0f, 0.0f},glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{1.0f,0.0f,0.0f}, glm::vec2{1.0f, 1.0f}},
		Vertex{glm::vec3{-1.0f, 1.0f, 0.0f},glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f,0.0f,1.0f}, glm::vec2{0.0f, 1.0f}}
	};

	//Define a lista de elementos que formam os triângulos
	std::array<glm::ivec3, 2> Indices = {
		glm::ivec3{0, 1, 3},
		glm::ivec3{3, 1, 2}
	};

	//copia os dados para a GPU
	GLuint VertexBuffer;

	//gera o identifcador do vertexbuffer - VBO
	glGenBuffers(1, &VertexBuffer);

	//pede para o opengl gerar o identificador do EBO
	GLuint ElementBuffer = 0;
	glGenBuffers(1, &ElementBuffer);

	//ativa o vertex como sendo o buffer para onde os dados vão ser copiados
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	//copias os dados para a memória de vídeo
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad.data(), GL_STATIC_DRAW);

	//copia os dados do Element Buffer para a GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices.data(), GL_STATIC_DRAW);

	//Gera o vertex array object - VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	//habilita o VAO
	glBindVertexArray(VAO);

	//Atributo para cor nos vértices
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//aponta para o OpenGl quaul vai ser o buffer ativo no momento
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	//informa onde dentro do buffer estão os vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);
	return VAO;
}

void GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>&Indices) {
	Vertices.clear();
	Indices.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	float InvResolution = 1.0f / static_cast<float>(Resolution - 1);

	for (GLuint UIndex = 0; UIndex < Resolution; ++UIndex) {
		const float U = UIndex * InvResolution;
		const float Theta = glm::mix(0.0f, Pi, U);

		for (GLuint VIndex = 0; VIndex < Resolution; ++VIndex) {
			const float V = VIndex * InvResolution;
			const float Phi = glm::mix(0.0f, TwoPi, V);

			glm::vec3 VertexPosition = {
				glm::sin(Theta) * glm::cos(Phi),
				glm::sin(Theta)* glm::sin(Phi),
				glm::cos(Theta)
			};

			Vertex Vertex{
				VertexPosition,
				glm::normalize(VertexPosition),
				glm::vec3{1.0f, 1.0f, 1.0f},
				glm::vec2{1.0f - U, V}
			};

			Vertices.push_back(Vertex);
		}
	}

	for (GLuint U = 0; U < Resolution - 1; ++U) {
		for (GLuint V = 0; V < Resolution - 1; ++V) {
			GLuint P0 = U + V * Resolution;
			GLuint P1 = (U + 1) + V * Resolution;
			GLuint P2 = (U + 1) + (V + 1) * Resolution;
			GLuint P3 = U + (V + 1) * Resolution;

			Indices.push_back(glm::ivec3{ P0, P1, P3 });
			Indices.push_back(glm::ivec3{ P3, P1, P2 });
		}
	}
}

GLuint LoadSphere(GLuint& NumVertices, GLuint& NumIndices) {
	std::vector<Vertex> Vertices;
	std::vector<glm::ivec3> Triangles;
	GenerateSphereMesh(50, Vertices, Triangles);

	NumVertices = Vertices.size();
	NumIndices = Triangles.size() * 3;

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);

	//ativa o vertex como sendo o buffer para onde os dados vão ser copiados
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	//copias os dados para a memória de vídeo
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	GLuint ElementBuffer;
	glGenBuffers(1, &ElementBuffer);

	//ativa o vertex como sendo o buffer para onde os dados vão ser copiados
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	//copias os dados para a memória de vídeo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint), Triangles.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Atributo para cor nos vértices
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);


	//aponta para o OpenGl quaul vai ser o buffer ativo no momento
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	//informa onde dentro do buffer estão os vértices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);
	return VAO;
}

class FlyCamera {
public:
	void MoveForward(float Amount) {
		LocationVRP += glm::normalize(Direction) * Amount * Speed;
	}

	void MoveRight(float Amount) {
		glm::vec3 Right = glm::normalize(glm::cross(Direction, ViewUp));
		LocationVRP += Right * Amount * Speed;
	}

	void Look(float Yaw, float Pitch) {
		Yaw *= Sensitivity;
		Pitch *= Sensitivity;

		const glm::vec3 Right = glm::normalize(glm::cross(Direction, ViewUp));

		const glm::mat4 I = glm::identity < glm::mat4>();
		glm::mat4 YawRotation = glm::rotate(I, glm::radians(Yaw), ViewUp);
		glm::mat4 PitchRotation = glm::rotate(I, glm::radians(Pitch), Right);

		ViewUp = PitchRotation * glm::vec4{ ViewUp, 0.0f };
		Direction = YawRotation * PitchRotation * glm::vec4{ Direction, 0.0f };
	}

	glm::mat4 GetView() const {
		return glm::lookAt(LocationVRP, LocationVRP + Direction, ViewUp);
	}

	glm::mat4 GetViewProjection() const {
		//glm::mat4 View = glm::lookAt(LocationVRP, LocationVRP + Direction, ViewUp);
		glm::mat4 Projection = glm::perspective(angulo_de_visao, razao_aspecto, near, far);
		return Projection * GetView();
	}

	//Parametros de Interatividade
	float Speed = 5.0f;
	float Sensitivity = 0.1f;

	//Definição da Matriz de View
	glm::vec3 LocationVRP{ 0.0f, 0.0f, 10.0f };
	glm::vec3 Direction{0.0f, 0.0f, -1.0f};
	glm::vec3 ViewUp{ 0.0f, 1.0f, 0.0f };

	//Definição da Matriz de projeção
	float angulo_de_visao = glm::radians(45.0f);
	float razao_aspecto = width / height;
	float near = 0.01f;
	float far = 1000.0f;
};

FlyCamera Camera;
bool bEnableMouseMovement = false;
glm::vec2 PreviousCursor{ 0.0, 0.0 };

void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers) {
	//std::cout << "Button: " << Button << " Action: " << Action << " Modifiers: " << Modifiers << std::endl;
	if (Button == GLFW_MOUSE_BUTTON_LEFT) {
		if (Action == GLFW_PRESS) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			double X, Y;
			glfwGetCursorPos(Window, &X, &Y);

			PreviousCursor = glm::vec2{ X,Y };
			bEnableMouseMovement = true;
		}

		else if (Action == GLFW_RELEASE) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bEnableMouseMovement = false;
		}
	}
}
 
void MouseMotionCallBack(GLFWwindow* Window, double X, double Y) {
	if (bEnableMouseMovement) {
		glm::vec2 CurrentCursor{ X, Y };
		glm::vec2 DeltaCursor =   CurrentCursor - PreviousCursor ;

		//std::cout << glm::to_string(DeltaCursor) << std::endl;

		Camera.Look(-DeltaCursor.x, -DeltaCursor.y);
		PreviousCursor = CurrentCursor;
	}		
}

void Resize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	width = NewWidth;
	height = NewHeight;

	Camera.razao_aspecto = static_cast<float>(width) / height;
	glViewport(0, 0, width, height);
}

int main() {
	//inicialização
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	//criar a janela
	GLFWwindow* Window = glfwCreateWindow(width, height, "Blue Marble", nullptr, nullptr);
	if (!Window){
		std::cout << "Erro ao criar janela" << std::endl;
		glfwTerminate();
		return 1;
	}

	//Cadastra as callbacks no GLFW
	glfwSetMouseButtonCallback(Window, MouseButtonCallback);
	glfwSetCursorPosCallback(Window, MouseMotionCallBack);
	glfwSetFramebufferSizeCallback(Window, Resize);

	//ativa o contexto criado na janela window
	glfwMakeContextCurrent(Window);

	//habilita e desabilita o v-sync
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//Obtem informações do driver
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	Resize(Window, width, height);

	// Compilar o vertex e o fragment shader
	GLuint ProgramID = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

	GLuint TextureID = LoadTexture("textures/earth_2k.jpg");
	GLuint CloudTextureID = LoadTexture("textures/earth_clouds_2k.jpg");

	GLuint QuadVAO = LoadGeometry();

	GLuint SphereNumVertices = 0;
	GLuint SphereNumIndices = 0;
	GLuint SphereVAO = LoadSphere(SphereNumVertices, SphereNumIndices);

	std::cout << "Numero de vertices da esfera: " << SphereNumVertices << std::endl;
	std::cout << "Numero de indices da esfera: " << SphereNumIndices << std::endl;

	//Model Matrix
	glm::mat4 I = glm::identity<glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 1,0,0 });

	//definição da cor de fundo em RGBA
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);

	//salva o tempo do frame anterior
	double PreviousTime = glfwGetTime();

	//habilita o backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//habilita o teste de porfundidade (Z-Buffer)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//criação da fonte de luz direcional
	DirectionalLight Light;
	Light.Direction = glm::vec3{ 0.0f, 0.0f, -1.0f };
	Light.Intensity = 1.0f;

	while(!glfwWindowShouldClose(Window)){
		
		double CurrentTime = glfwGetTime();
		double DeltaTime = CurrentTime - PreviousTime;
		if (DeltaTime > 0.0) {
			PreviousTime = CurrentTime;
		}
		
		//limpa o buffer de cor e preenche com a for configurada
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Ativar o programa de shader
		glUseProgram(ProgramID);

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 ViewProjectionMatrix = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjectionMatrix * ModelMatrix; 

		GLint TimeLoc = glGetUniformLocation(ProgramID, "Time");
		glUniform1f(TimeLoc, CurrentTime);

		GLint ModelViewProjectionLoc = glGetUniformLocation(ProgramID, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		GLint NormalMatrixLoc = glGetUniformLocation(ProgramID, "NormalMatrix");
		glUniformMatrix4fv(NormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, CloudTextureID);

		GLint TextureSamplerLoc = glGetUniformLocation(ProgramID, "TextureSampler");
		glUniform1i(TextureSamplerLoc, 0);

		GLint CloudTextureLoc = glGetUniformLocation(ProgramID, "CloudsTexture");
		glUniform1i(CloudTextureLoc, 1);
		
		GLint LightDirectionLoc = glGetUniformLocation(ProgramID, "LightDirection");
		glUniform3fv(LightDirectionLoc, 1, glm::value_ptr(Camera.GetView()* glm::vec4{ Light.Direction, 0.0f }));

		GLint LightIntensityLoc = glGetUniformLocation(ProgramID, "LihgtIntensity");
		glUniform1f(LightIntensityLoc, Light.Intensity);

		glBindVertexArray(SphereVAO);

		//desenha o objeto com os dados armazenados no vertexbuffer
		glPointSize(10.0f);
		glLineWidth(10.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, SphereNumIndices, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		//Desabilita o programa ativo
		glUseProgram(0);

		//Processamento de todos os eventos da fila
		glfwPollEvents();

		//Envia o conteúdo para ser desenhado
		glfwSwapBuffers(Window);

		//Processamento dos inputs do teclado
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
			Camera.MoveForward(1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
			Camera.MoveForward(-1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
			Camera.MoveRight(-1.0f * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
			Camera.MoveRight(1.0f * DeltaTime);
		}
	}

	//desaloca o buffer
	glDeleteVertexArrays(1, &QuadVAO);

	//encerra o glfw
	glfwTerminate();

	return 0;
}