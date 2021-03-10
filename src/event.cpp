#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<game.h>
#include<cmath>
#include<ctime>
#include<irrKlang.h>
using namespace std;

extern GLFWwindow* window;
extern vec3 camPos;
extern float camYaw, camPitch;

extern short playerInventory[41];
extern int currentItemSlot;


extern irrklang::ISoundEngine* soundEngine;

bool Mouse1isPressed = false, Mouse2isPressed = false;
void onUpdate() {	//the function updates the general status of the game
    tickUpdate();
    //textureUpdate();
    processInput(window);
    


    extern float camPitch, camYaw;
    float Dy = sinf(deg2rad(camPitch));
    float Dx = cosf(deg2rad(camYaw)) * cosf(deg2rad(camPitch));
    float Dz = sinf(deg2rad(camYaw)) * cosf(deg2rad(camPitch));
    bool getBlockFlag = false;
    vec3 tmp;
    int i = 1;
    vec3 D(Dx, Dy, Dz);
    for (; i <= 40; i++) { //raycasting
        float norm = norm3fv(D);
        D.x /= norm; D.y /= norm; D.z /= norm;
        tmp = D * i * 0.1f + camPos;
        tmp.x = roundf(tmp.x); tmp.y = roundf(tmp.y); tmp.z = roundf(tmp.z);    //the blocks are rendered -0.5~+0.5 relatively
        int blockID = getBlockID(tmp.x, tmp.y, tmp.z);
        if (blockID != 0 && blockID != -1 && blockID != 9) {
            getBlockFlag = true;
            drawBlock(tmp.x, tmp.y, tmp.z, 49);
            break;
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (getBlockFlag && !Mouse1isPressed) {
            int ItemToDelete = getBlockID(tmp.x, tmp.y, tmp.z);


            switch (ItemToDelete) {
            case 1:
                soundEngine->play2D("MSP/dig/coral3.ogg");
                break;
            case 2:
                soundEngine->play2D("MSP/dig/gravel3.ogg");
                break;
            case 3:
                soundEngine->play2D("MSP/dig/gravel3.ogg");
                break;
            case 4:
                soundEngine->play2D("MSP/dig/coral1.ogg");
                break;
            case 5:
                soundEngine->play2D("MSP/dig/wood1.ogg");
                break;
            case 7:
                soundEngine->play2D("MSP/dig/coral3.ogg");
                break;
            case 12:
                soundEngine->play2D("MSP/dig/sand1.ogg");
                break;
            case 13:
                soundEngine->play2D("MSP/dig/gravel1.ogg");
                break;
            case 14:
                soundEngine->play2D("MSP/dig/coral3.ogg");
                break;
            case 15:
                soundEngine->play2D("MSP/dig/coral3.ogg");
                break;
            case 16:
                soundEngine->play2D("MSP/dig/coral3.ogg");
                break;
            case 17:
                soundEngine->play2D("MSP/dig/wood1.ogg");
                break;

            }
            deleteBlock(tmp.x, tmp.y, tmp.z);
            Mouse1isPressed = 1;
        }
        //cout << "MOUSE1";
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS)
        Mouse1isPressed = false;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        if (getBlockFlag && !Mouse2isPressed) {
            vec3 OGtmp = D * i * 0.1f + camPos;     //original
            vec3 offset = OGtmp - tmp;      //calculate the relative offset to the center of the block;
            if (offset.y > abs(offset.x) && offset.y > abs(offset.z))       //dual LP
                addBlock(tmp.x, tmp.y + 1, tmp.z, playerInventory[currentItemSlot]);
            else if (offset.y < -abs(offset.x) && offset.y < -abs(offset.z))
                addBlock(tmp.x, tmp.y - 1, tmp.z, playerInventory[currentItemSlot]);
            else if (offset.x > abs(offset.y) && offset.x > abs(offset.z))
                addBlock(tmp.x + 1, tmp.y, tmp.z, playerInventory[currentItemSlot]);
            else if (offset.x < -abs(offset.y) && offset.x < -abs(offset.z))
                addBlock(tmp.x - 1, tmp.y, tmp.z, playerInventory[currentItemSlot]);
            else if (offset.z > abs(offset.y) && offset.z > abs(offset.x))
                addBlock(tmp.x, tmp.y, tmp.z + 1, playerInventory[currentItemSlot]);
            else if (offset.z < -abs(offset.y) && offset.z < -abs(offset.x))
                addBlock(tmp.x, tmp.y, tmp.z - 1, playerInventory[currentItemSlot]);

            //addBlock(tmp.x, tmp.y, tmp.z, playerInventory[currentItemSlot]);
        }
        Mouse2isPressed = 1;
        //cout << "MOUSE2";
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) != GLFW_PRESS)
        Mouse2isPressed = false;


}

int seed_addition = 1;
extern int currentItemSlot;
void processInput(GLFWwindow* window)
{
    extern vec3 D;
    extern vec3 R;
    int Dy = D.y;
    D.y = 0;    //restrict up-down
    R.y = 0;
    float tmpD = norm3fv(D);    D.x /= tmpD; D.z /= tmpD;
    float tmpR = norm3fv(R);    R.x /= tmpR; R.z /= tmpR;
    float cameraSpeed = 0.05f;

    extern int FOV_realtime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        cameraSpeed = 0.30f;    //sprint speed
        //FOV_realtime = 1.1 * FOVgame;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camPos = camPos + D * cameraSpeed;
        //camPos.x += cameraSpeed * sinf(deg2rad(camYaw));
        //camPos.z += cameraSpeed * cosf(deg2rad(camYaw));
    }

#ifndef NO_VERBOSE
        cout << "Event: Key \"W\" is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camPos = camPos - D * cameraSpeed;
    }

#ifndef NO_VERBOSE
        cout << "Event: Key \"S\" is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camPos = camPos - R * cameraSpeed;
        //camPos.x -= cameraSpeed * R.x;  //the R is actually left
        //camPos.z -= cameraSpeed * R.z;
    }

#ifndef NO_VERBOSE
        cout << "Event: Key \"A\" is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camPos = camPos + R * cameraSpeed;
    }
#ifndef NO_VERBOSE
        cout << "Event: Key \"D\" is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camPos.y += cameraSpeed;
    }

#ifndef NO_VERBOSE
        cout << "Event: SPACE is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camPos.y -= cameraSpeed;
    }

#ifndef NO_VERBOSE
        cout << "Event: LSHIFT is pressed." << endl;
        cout << "camPos.x:" << camPos.x << " camPos.y:" << camPos.y << " camPos.z:" << camPos.z << endl;
#endif


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        cout << "Escape was pressed, leaving." << endl;
    }

#ifdef DEV_MODE
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        cout << "Switched to GL_LINE mode." << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        cout << "Switched to GL_FILL mode." << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        generateWorldPerlin(SEED + seed_addition++);
#endif

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        currentItemSlot = 10;       //0 is abondoned
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        currentItemSlot = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        currentItemSlot = 2;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        currentItemSlot = 3;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        currentItemSlot = 4;
    }

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        currentItemSlot = 5;
    }

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        currentItemSlot = 6;
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        currentItemSlot = 7;
    }

    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        currentItemSlot = 8;
    }

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        currentItemSlot = 9;
    }


    D.y = Dy;
}

void tickUpdate() {
    tick = clock() * TPS / 1000;
}