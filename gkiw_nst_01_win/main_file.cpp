/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <iostream>

float speed = 3.14;

float angle_x = 1;
float angle_y = 1;
float angle_z = 1;

float fovy = 50.0f;
float aspect = 1.0f;
float near = 1.0f;
float far = 50.0f;

float camera_x = 0.0f;
float camera_y = 0.0f;
float camera_z = 5.0f;

float point_x = 0.0f, point_y = 0.0f, point_z = 0.0f;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_U && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) camera_x -= 1.00f; else camera_x += 1.00f;
	if (key == GLFW_KEY_I && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) camera_y -= 1.00f; else camera_y += 1.00f;
	if (key == GLFW_KEY_O && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) camera_z -= 1.00f; else camera_z += 1.00f;

	if (key == GLFW_KEY_J && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) point_x -= 1.00f; else point_x += 1.00f;
	if (key == GLFW_KEY_K && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) point_y -= 1.00f; else point_y += 1.00f;
	if (key == GLFW_KEY_L && action == GLFW_PRESS) if (mods & GLFW_MOD_SHIFT) point_z -= 1.00f; else point_z += 1.00f;


	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		std::cout << "camera_x = " << camera_x << std::endl;
		std::cout << "camera_y = " << camera_y << std::endl;
		std::cout << "camera_z = " << camera_z << std::endl;
		std::cout << "point_x = " << point_x << std::endl;
		std::cout << "point_y = " << point_y << std::endl;
		std::cout << "point_z = " << point_z << std::endl;
	}


	if (action == GLFW_RELEASE) {
		speed = 0;
	}

}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************	
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(window, key_callback);
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************	
}



//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l

	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// macierz rzutowania perspektywicznego
	//glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, near, far);

	// macierz widoku
	glm::mat4 V = glm::lookAt(
		//glm::vec3(0.0f, 0.0f, 5.0f),
		glm::vec3(camera_x, camera_y, camera_z),
		glm::vec3(point_x, point_y, point_z),
		glm::vec3(0.0f, 1.0f, 0.0f));

	spLambert->use();//Aktywacja programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.2f, 0.4f, 1);

	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

	// TORUS 1
	glm::mat4 M = glm::mat4(1.0f);

	//M = glm::translate(M, glm::vec3(1.05f, 0.0f, 0.0f));
	//M = glm::rotate(M,
	//	angle, glm::vec3(0.0f, 0.0f, 1.0f));

	//Załadowanie macierzy modelu do programu cieniującego
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(spLambert->u("color"), 0, 1, 0, 1);

	// właściwe rysowanie
	Models::cube.drawWire();

	// to ma byc na samym koncu
	glfwSwapBuffers(window);

}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle = 0;
	glfwSetTime(0);

	//Główna pętla	
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle += speed * glfwGetTime();
		glfwSetTime(0);

		drawScene(window, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
