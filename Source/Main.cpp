#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "../Header/Util.h"
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
    float leftBgVertices[] = {
        -1.0f,  1.0f,
         0.0f,  1.0f,
         0.0f, -1.0f,
        -1.0f, -1.0f
    };
    GLuint leftBgVAO, leftBgVBO;
    glGenVertexArrays(1, &leftBgVAO);
    glGenBuffers(1, &leftBgVBO);
    glBindVertexArray(leftBgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leftBgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftBgVertices), leftBgVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desna polovinu (svetlo siva)
    float rightBgVertices[] = {
         0.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
         0.0f, -1.0f
    };
    GLuint rightBgVAO, rightBgVBO;
    glGenVertexArrays(1, &rightBgVAO);
    glGenBuffers(1, &rightBgVBO);
    glBindVertexArray(rightBgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightBgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightBgVertices), rightBgVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --------------------- Levi panel kvadrati ---------------------
    float panelLeft = -1.0f;
    float panelRight = 0.0f;
    float panelBottom = -1.0f;
    float panelTop = 1.0f;

    int rows = 6;
    int cols = 2;
    float buttonWidth = 0.18f;
    float buttonHeight = 0.18f;
    float hSpacing = 0.05f;
    float vSpacing = 0.05f;

    float totalWidth = cols * buttonWidth + (cols - 1) * hSpacing;
    float totalHeight = rows * buttonHeight + (rows - 1) * vSpacing;

    vector<GLuint> panelVAOs(rows * cols);
    vector<GLuint> panelVBOs(rows * cols);
    glGenVertexArrays(panelVAOs.size(), panelVAOs.data());
    glGenBuffers(panelVBOs.size(), panelVBOs.data());

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;

            // horizontalno centriranje
            float x0 = panelLeft + (panelRight - panelLeft - totalWidth) / 2 + j * (buttonWidth + hSpacing);
            float x1 = x0 + buttonWidth;

            // vertikalno centriranje
            float y0 = panelBottom + (panelTop - panelBottom - totalHeight) / 2 + (rows - 1 - i) * (buttonHeight + vSpacing);
            float y1 = y0 + buttonHeight;

            float vertices[] = { x0, y0, x1, y0, x1, y1, x0, y1 };

            glBindVertexArray(panelVAOs[idx]);
            glBindBuffer(GL_ARRAY_BUFFER, panelVBOs[idx]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
    }


    // --------------------- Desna polovina - spratovi ---------------------
    vector<GLuint> floorVAOs(8);
    vector<GLuint> floorVBOs(8);
    glGenVertexArrays(8, floorVAOs.data());
    glGenBuffers(8, floorVBOs.data());

    for (int i = 0; i < 8; i++) {
        float y = -0.9f + i * 0.225f;
        float vertices[] = {
            0.0f, y,   // levo (pola ekrana)
            1.0f, y    // desno
        };
        glBindVertexArray(floorVAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

    }

    // Lift (desna strana)
    float liftWidth = 0.1f, liftHeight = 0.227f;
    float liftX0 = 0.9f; // desna ivica ekrana
    float liftX1 = liftX0 + liftWidth;
    float liftY0 = -0.9f; // prvi sprat
    float liftY1 = liftY0 + liftHeight;
    float liftVertices[] = { liftX0,liftY0, liftX1,liftY0, liftX1,liftY1, liftX0,liftY1 };
    GLuint liftVAO, liftVBO;
    glGenVertexArrays(1, &liftVAO);
    glGenBuffers(1, &liftVBO);
    glBindVertexArray(liftVAO);
    glBindBuffer(GL_ARRAY_BUFFER, liftVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(liftVertices), liftVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //-------------------------------------------
    // Osoba u desnom panelu (levi deo desnog panela)
    //-------------------------------------------

    // granice desnog panela
    float rightPanelLeft = 0.0f;
    float rightPanelRight = 1.0f;

    float personWidth = 0.10f;   // širi malo zbog vidljivosti
    float personHeight = 0.18f;

    // osoba bude levo poravnata unutar desnog panela
    float personX0 = rightPanelLeft;  // 0.1 = margina od ivice panela
    float personX1 = personX0 + personWidth;

    // neka osoba bude dole u desnom panelu
    float personY0 = -0.9f;
    float personY1 = personY0 + personHeight;

    float personVertices[] = {
        personX0, personY0,
        personX1, personY0,
        personX1, personY1,
        personX0, personY1
    };

    GLuint personVAO, personVBO;
    glGenVertexArrays(1, &personVAO);
    glGenBuffers(1, &personVBO);

    glBindVertexArray(personVAO);
    glBindBuffer(GL_ARRAY_BUFFER, personVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(personVertices), personVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        // ESC izlazak
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // pozadina
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        // 1️⃣ Pozadina
        glUniform3f(glGetUniformLocation(shader, "uColor"), 0.3f, 0.2f, 0.3f); // leva
        glBindVertexArray(leftBgVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform3f(glGetUniformLocation(shader, "uColor"), 0.8f, 0.8f, 0.8f); // desna
        glBindVertexArray(rightBgVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // 2️⃣ Dugmići
        glUniform3f(glGetUniformLocation(shader, "uColor"), 0.6f, 0.6f, 0.6f);
        for (auto vao : panelVAOs) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        // 3️⃣ Spratovi
        glUniform3f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f);
        for (auto vao : floorVAOs) {
            glBindVertexArray(vao);
            glDrawArrays(GL_LINES, 0, 2);
        }

        // 4️⃣ Lift
        glUniform3f(glGetUniformLocation(shader, "uColor"), 1.0f, 0.0f, 0.0f);
        glBindVertexArray(liftVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // 5️⃣ Osoba
        glUniform3f(glGetUniformLocation(shader, "uColor"), 0.0f, 0.0f, 1.0f);
        glBindVertexArray(personVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}