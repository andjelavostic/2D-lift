#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "../Header/Util.h"

#include "../Panel.h"
#include "../PanelGrid.h"
#include "../Elevator.h"
#include "../Person.h"
using namespace std;
// Main fajl funkcija sa osnovnim komponentama OpenGL programa

// Projekat je dozvoljeno pisati počevši od ovog kostura
// Toplo se preporučuje razdvajanje koda po fajlovima (i eventualno potfolderima) !!!
// Srećan rad!
int main()
{
    glfwSetErrorCallback([](int code, const char* msg) {
        std::cout << "GLFW GREŠKA (" << code << "): " << msg << std::endl;
        });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // Dohvati monitor za fullscreen
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Kreiraj fullscreen prozor
    GLFWwindow* window = glfwCreateWindow(
        mode->width, mode->height,
        "Lift Simulator",
        monitor,
        NULL
    );
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    unsigned int shader = createShader("basic.vert", "basic.frag");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Crtaj levu polovinu (tamno siva)
    Panel leftPanel(-1.0f, 0.0f, -1.0f, 1.0f, 0.3f, 0.2f, 0.3f);
   
    // Desna polovinu (svetlo siva)
    Panel rightPanel(0.0f, 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.8f);

    // --------------------- Levi panel kvadrati ---------------------
    PanelGrid leftPanelButtons(
        -1.0f, 0.0f, -1.0f, 1.0f, // pozicija panela
        6, 2,                     // rows, cols
        0.18f, 0.18f,             // buttonWidth, buttonHeight
        0.05f, 0.05f              // horizontal i vertikalni razmak
    );

    //  SPRATOVI – ravnomerno raspoređeni (suteren do 6. sprata)
    Elevator elevator;
    Person person(1, elevator.getFloorSpacing());

    
    bool liftCalled = false;
    int targetFloor = 0;
    bool doorsOpen = false;
    float doorDuration = 5.0f;
    float doorOpenTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // ESC izlazak
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // pozadina
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        // -------- Kretanje osobe levo-desno --------
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) person.moveLeft();
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) person.moveRight(elevator.getLiftX0());

        // poziv lifta
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (person.touchesLift(elevator.getLiftX0())) {
                elevator.callLift(person.getFloor());
            }
        }

        elevator.updateLift();

        // -------- Zatvaranje vrata --------
        if (doorsOpen && (glfwGetTime() - doorOpenTime > doorDuration)) {
            doorsOpen = false;
        }

        // -------- Crtanje scene --------
        leftPanel.draw(shader);
        rightPanel.draw(shader);
        leftPanelButtons.draw(shader);
        elevator.drawFloors(shader);
        elevator.drawLift(shader, doorsOpen);
        person.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}