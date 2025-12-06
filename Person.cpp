#include "Person.h"
#include <GL/glew.h>
#include "../2D-lift/Header/Util.h"
Person::Person(int startFloor, float floorSpacing, float w, float heightFactor, float startX) {
    width = w;
    height = floorSpacing * heightFactor;
    floor = startFloor;
    posX = startX;

    x0 = startX;
    x1 = x0 + width;
    y0 = -1.0f + floor * floorSpacing;
    y1 = y0 + height;

    float vertices[] = {
        // x, y, u, v
        x0, y0, 0.0f, 0.0f,
        x1, y0, 1.0f, 0.0f,
        x1, y1, 1.0f, 1.0f,
        x0, y1, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // pozicija
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    personSpeed = 0.001f;
    texPerson = loadImageToTexture("resources/person.png");
}

void Person::draw(GLuint shader) {
    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texPerson);
    glUniform1i(glGetUniformLocation(shader, "uTexture"), 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);

    GLuint offsetLoc = glGetUniformLocation(shader, "uOffset");
    glUniform2f(offsetLoc, posX, 0.0f);

    // update UV dinamicki
    float vertices[] = {
        x0, y0, u0, 0.0f,
        x1, y0, u1, 0.0f,
        x1, y1, u1, 1.0f,
        x0, y1, u0, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0); // vracamo boju za ostalo
}

void Person::moveLeft(float liftX0, bool doorsOpen)
{
    posX -= personSpeed;

    if (isInsideLift) {
        // izlazak moguc samo kada su vrata otvorena
        float personLeft = x0 + posX;
        if (!doorsOpen) {
            if (personLeft < liftX0)
                posX = liftX0 - x0;   // ne moze napolje
        }
        else {
            // vrata otvorena  moze izaci
            if (personLeft < 0.0f) posX = 0.0f;
            if (personLeft < liftX0)
                isInsideLift = false; // izasla je
        }
    }
    else {
        // normalno levo pre nego sto udje
        if (posX < 0.0f) posX = 0.0f;
    }

    facingRight = false;
    u0 = 1.0f; u1 = 0.0f;
}
void Person::moveRight(float liftX0, float liftX1, bool doorsOpen, int liftFloor)
{
    bool sameFloor = (floor == liftFloor);

    // Pomeri prvo
    posX += personSpeed;

    float personRight = x0 + posX + width;
    float personLeft = x0 + posX;

    if (!isInsideLift) {
        // ulazak u lift moguc samo kada su vrata otvorena i osoba na istom spratu
        if (sameFloor && doorsOpen) {
            if (personRight >= liftX0) {
                isInsideLift = true; // osoba ulazi u lift
            }
        }

        // ogranicenja ako lift zatvoren ili osoba ispred lifta
        if (!sameFloor || !doorsOpen) {
            if (personRight > liftX0)
                posX = liftX0 - width - x0; // ne sme dalje
        }
        else {
            // osoba u liftu  ogranicenje desno
            if (personRight > liftX1)
                posX = liftX1 - width - x0;
        }
    }
    else {
        // osoba je vec u liftu  desno ne sme van lifta
        float rightLimit = liftX1;
        if (personRight > rightLimit)
            posX = rightLimit - width - x0;

        // levo može slobodno (kontroliše moveLeft)
    }

    facingRight = true;
    u0 = 0.0f; u1 = 1.0f;
}

bool Person::touchesLift(float liftX0)
{
    float personRight = x0 + posX + width;
    return personRight >= liftX0;
}
void Person::syncWithLift(float liftY0,int liftFloor)
{
    if (isInsideLift) {
        y0 = liftY0;  // osoba prati lift
        floor = liftFloor;
    }

}




