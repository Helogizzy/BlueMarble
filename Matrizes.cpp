#include <iostream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void PrintMatrix(const glm::mat4& M){	
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j){
			std::cout 
				<< std::setw(10) 
				<< std::setprecision(4) 
				<< std::fixed
				<< M[i][j] << " ";
		}

		std::cout << std::endl;
	}
}

void TranslationMatrix(){
	std::cout << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Matriz de Translacao" << std::endl;
	std::cout << "==================" << std::endl;
	

	glm::vec4 Position{ 10, 10, 10, 1 };
	glm::vec4 Direction{ 10, 10, 10, 0 };
	glm::vec3 Translate{ 10, 10, 10 };
	glm::mat4 Translation = glm::translate(glm::identity<glm::mat4>(), Translate);

	PrintMatrix(Translation);

	std::cout << std::endl;
	Position = Translation * Position;
	Direction = Translation * Direction;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

void RotationMatrix(){
	std::cout << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Matriz de Rotacao" << std::endl;
	std::cout << "==================" << std::endl;

	glm::vec4 Position{ 100, 0, 0, 1 };
	glm::vec4 Direction{ 100, 0, 0, 0 };
	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f), Axis);

	PrintMatrix(Rotation);

	std::cout << std::endl;
	Position = Rotation * Position;
	Direction = Rotation * Direction;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

void ScaleMatrix(){
	std::cout << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Matriz de Escala" << std::endl;
	std::cout << "==================" << std::endl;

	glm::vec4 Position{ 100, 100, 0, 1 };
	glm::vec4 Direction{ 100, 100, 0, 0 };
	glm::vec3 ScaleAmount{ 2, 2, 2 };
	glm::mat4 Scale = glm::scale(glm::identity<glm::mat4>(), ScaleAmount);

	PrintMatrix(Scale);

	std::cout << std::endl;
	Position = Scale * Position;
	Direction = Scale * Direction;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

void ComposedMatrix(){
	std::cout << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Composicao" << std::endl;
	std::cout << "==================" << std::endl;

	glm::vec4 Position{ 1, 1, 0, 1 };
	glm::vec4 Direction{ 1, 1, 0, 0 };

	glm::vec3 Translate{ 0, 10, 0 };
	glm::mat4 Translation = glm::translate(glm::identity<glm::mat4>(), Translate);

	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(45.0f), Axis);

	glm::vec3 ScaleAmount{ 2, 2, 0 };
	glm::mat4 Scale = glm::scale(glm::identity<glm::mat4>(), ScaleAmount);

	std::cout << "Translacao: " << std::endl;
	PrintMatrix(Translation);
	std::cout << std::endl;

	std::cout << "Rotacao: " << std::endl;
	PrintMatrix(Rotation);
	std::cout << std::endl;

	std::cout << "Escala: " << std::endl;
	PrintMatrix(Scale);
	std::cout << std::endl;

	glm::mat4 Transform = Translation * Rotation * Scale;
	
	std::cout << "Transformacao: " << std::endl;
	PrintMatrix(Transform);
	std::cout << std::endl;

	Position = Transform * Position;
	Direction = Transform * Direction;

	std::cout << std::endl;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;	
}

void ModelViewProject() {
	std::cout << std::endl;
	std::cout << "==================" << std::endl;
	std::cout << "Modelo de Projecao de Vista" << std::endl;
	std::cout << "==================" << std::endl;

	//Matriz Composta
	glm::mat4 ModelMatrix = glm::identity<glm::mat4>();

	//View
	glm::vec3 VRP{0, 0, 10};
	glm::vec3 PontoFocal{0, 0, 0};
	glm::vec3 ViewUp{0, 1, 0};
	glm::mat4 ViewMatrix = glm::lookAt(VRP, PontoFocal, ViewUp);

	std::cout << "View" << std::endl;
	PrintMatrix(ViewMatrix);

	constexpr float angulo_de_visao = glm::radians(45.0f);
	const float razao_aspecto = 800.0f / 600.0f;
	const float near = 0.001f;
	const float far = 1000.0f;
	glm::mat4 ProjectionMatrix = glm::perspective(angulo_de_visao, razao_aspecto, near, far);

	std::cout << "Projection" << std::endl;
	PrintMatrix(ProjectionMatrix);

	glm::mat4 ModelViewProjection = ProjectionMatrix * ViewMatrix * ModelMatrix;

	std::cout << "ModelViewProjection" << std::endl;
	PrintMatrix(ModelViewProjection);

	glm::vec4 Position{ 0, 0 , 0, 1 };
	Position = ModelViewProjection * Position;

	Position - Position.w;

	std::cout << glm::to_string(Position) << std::endl;

}

int main(){		
	TranslationMatrix();
	RotationMatrix();
	ScaleMatrix();
	ComposedMatrix();
	ModelViewProject();

	return 0;
}