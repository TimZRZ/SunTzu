#include "CameraMove.h"
#include "Matrix.h"

int const scale = 2;

void verticalRotate(STCamera* camera, int deltaX) {
    // Move lookat to (0, 0), later move back
    float tempC[4] = {
        camera->position[0] - camera->lookat[0],
        camera->position[1] - camera->lookat[1],
        camera->position[2] - camera->lookat[2],
        1
    };

    float orbit[4] = {
        camera->worldup[0],
        camera->worldup[1],
        camera->worldup[2],
        1
    };

    float cosP = orbit[2] / sqrt(pow(orbit[0], 2) + pow(orbit[2], 2));
    float sinP = orbit[0] / sqrt(pow(orbit[0], 2) + pow(orbit[2], 2));

    if (orbit[2] == 0) {
        cosP = 1;
        sinP = 0;
    }

    STMatrix orbitToY0 = {
        {cosP, 0, -sinP, 0},
        {0, 1, 0, 0},
        {sinP, 0, cosP, 0},
        {0, 0, 0, 1}
    };
    float cosO = orbit[1];
    float sinO = sqrt(1 - pow(cosO, 2));
    STMatrix orbitToY1 = {
        {1, 0, 0, 0},
        {0, cosO, -sinO, 0},
        {0, sinO, cosO, 0},
        {0, 0, 0, 1}
    };
    STMatrix orbitTotal;
    crossMatrix(&orbitTotal, orbitToY1, orbitToY0);

    STMatrix orbitTotalReverse;
    rotationReverse(&orbitTotalReverse, orbitTotal);

    float angle = (deltaX / scale) * PI / 180.0;
    STMatrix rotate = {
        {cos(angle), 0, sin(angle), 0},
        {0, 1, 0, 0},
        {-sin(angle), 0, cos(angle), 0},
        {0, 0, 0, 1}
    };

    float rotateOrbit4Pos[4];
    crossMatrix(rotateOrbit4Pos, orbitTotal, tempC, 4);

    float roatedPos[4];
    crossMatrix(roatedPos, rotate, rotateOrbit4Pos, 4);

    camera->position[0] = dot4(orbitTotalReverse[0], roatedPos) + camera->lookat[0];
    camera->position[1] = dot4(orbitTotalReverse[1], roatedPos) + camera->lookat[1];
    camera->position[2] = dot4(orbitTotalReverse[2], roatedPos) + camera->lookat[2];
}

void horizontalRotate(STCamera* camera, int deltaY) {

    /* Deal with camera position */

    // Move lookat to (0, 0), later move back
    float tempC[4] = {
        camera->position[0] - camera->lookat[0],
        camera->position[1] - camera->lookat[1],
        camera->position[2] - camera->lookat[2],
        1
    };

    float tempW[4] = {
        camera->worldup[0] + tempC[0],
        camera->worldup[1] + tempC[1],
        camera->worldup[2] + tempC[2],
        1
    };

    float orbit[4] = {0, 0, 0, 1};
    if (tempC[0] == 0) {
        orbit[0] = 1;
    } else {
        if (tempC[0] > 0 && tempC[2] > 0) {
            orbit[2] = sqrt(pow(tempC[0], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
            orbit[0] = -sqrt(pow(tempC[2], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
        } else if (tempC[0] < 0 && tempC[2] > 0) {
            orbit[2] = -sqrt(pow(tempC[0], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
            orbit[0] = -sqrt(pow(tempC[2], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
        }
        else if (tempC[0] < 0 && tempC[2] < 0) {
            orbit[2] = -sqrt(pow(tempC[0], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
            orbit[0] = sqrt(pow(tempC[2], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
        } else {
            orbit[2] = sqrt(pow(tempC[0], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
            orbit[0] = -sqrt(pow(tempC[2], 2) / (pow(tempC[0], 2) + pow(tempC[2], 2)));
        }
    }



    STMatrix orbitToZ = {
        {orbit[2], 0, -orbit[0], 0},
        {0, 1, 0, 0},
        {orbit[0], 0, orbit[2], 0},
        {0, 0, 0, 1}
    };

    STMatrix orbitToZReverse = {
        {orbit[2], 0, orbit[0], 0},
        {0, 1, 0, 0},
        {-orbit[0], 0, orbit[2], 0},
        {0, 0, 0, 1}
    };

    float angle = - (deltaY / scale) * PI / 180.0;
    STMatrix rotate = {
        {cos(angle), -sin(angle), 0, 0},
        {sin(angle), cos(angle), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float rotateOrbit4Pos[4];
    crossMatrix(rotateOrbit4Pos, orbitToZ, tempC, 4);
    float rotateOrbit4WorldUp[4];
    crossMatrix(rotateOrbit4WorldUp, orbitToZ, tempW, 4);


    float roatedPos[4];
    crossMatrix(roatedPos, rotate, rotateOrbit4Pos, 4);
    float rotateWorldUp[4];
    crossMatrix(rotateWorldUp, rotate, rotateOrbit4WorldUp, 4);

    camera->position[0] = dot4(orbitToZReverse[0], roatedPos) + camera->lookat[0];
    camera->position[1] = dot4(orbitToZReverse[1], roatedPos) + camera->lookat[1];
    camera->position[2] = dot4(orbitToZReverse[2], roatedPos) + camera->lookat[2];

    camera->worldup[0] = dot4(orbitToZReverse[0], rotateWorldUp) - camera->position[0];
    camera->worldup[1] = dot4(orbitToZReverse[1], rotateWorldUp) - camera->position[1];
    camera->worldup[2] = dot4(orbitToZReverse[2], rotateWorldUp) - camera->position[2];
}