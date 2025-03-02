#include <iostream>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void Constructors(){
	std::cout << std::endl;

	glm::vec3 Point1{ 10.0f, 0.0f, 0.0f };
	glm::vec3 Point2 = glm::vec3(10.0f, 0.0f, 0.0f);
	glm::vec3 Point3 = { 10.0f, 0.0f, 0.0f };

	std::cout << "Point1: " << glm::to_string(Point1) << std::endl;
	std::cout << "Point2: " << glm::to_string(Point2) << std::endl;
	std::cout << "Point3: " << glm::to_string(Point3) << std::endl;
}

void Components(){
	std::cout << std::endl;

	glm::vec3 Point1{ 10.0f, 10.0f, 0.0f };
	std::cout << "X: " << Point1.x << " Y: " << Point1.y << " Z: " << Point1.z << std::endl;
	std::cout << "R: " << Point1.r << " G: " << Point1.g << " B: " << Point1.b << std::endl;	
	std::cout << "S: " << Point1.s << " T: " << Point1.t << " Q: " << Point1.p << std::endl;
	std::cout << "0: " << Point1[0] << " 1: " << Point1[1] << " 2: " << Point1[2] << std::endl;
}

void Swizzles(){
	// Precisa definir GLM_SWIZZLE
	std::cout << std::endl;

	glm::vec3 Point1{ 10.0f, 10.0f, 0.0f };
	glm::vec3 Point2 = Point1.xxx;
	glm::vec3 Point3 = Point1.bgr;
	glm::vec4 Point4 = Point1.bbbb;
	std::cout << "Point1: " << glm::to_string(Point1) << std::endl;
	std::cout << "Point2: " << glm::to_string(Point2) << std::endl;
	std::cout << "Point3: " << glm::to_string(Point3) << std::endl;
	std::cout << "Point4: " << glm::to_string(Point4) << std::endl;
}

void Operations(){
	glm::vec3 Point1{ 10.0f, 10.0f, 0.0f };
	glm::vec3 Point2{ 10.0f, 10.0f, 10.0f };
	
	// Soma
	glm::vec3 Point3 = Point1 + Point2;
	glm::vec3 Point4 = Point1 - Point2;

	// Scala
	glm::vec3 Point5 = Point1 * 10.0f;
	glm::vec3 Point6 = Point1 / 10.0f;

	// Comprimento
	float L = glm::length(Point1);
	// Não confundir com a função membro length
	constexpr int C = Point1.length();

	// Norma
	glm::vec3 Norm = glm::normalize(Point1);

	// Dot product
	float Dot = glm::dot(Point1, Point2);

	// Cross product
	glm::vec3 Cross = glm::cross(Point1, Point2);

	// Distance
	float Distance = glm::distance(Point1, Point2);

	// Refract
	glm::vec3 Refract = glm::refract(Point1, Norm, 1.0f);

	// Reflect
	glm::vec3 Reflect = glm::reflect(Point1, Norm);
}

int main(){
	Constructors();
	Components();
	Swizzles();
	Operations();

	return 0;
}