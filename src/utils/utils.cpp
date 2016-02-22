//
// Created by mehdi on 18/02/16.
//

#include "utils/utils.h"
#include <iostream>

void printMat4(const glm::mat4 &matrix) {
    std::cout << "[";
    std::cout << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] <<std::endl;
    std::cout << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] <<std::endl;
    std::cout << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] <<std::endl;
    std::cout << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[2][3] <<std::endl;
    std::cout << "]";
    std::cout << std::endl;
}

void printVec3(const glm::vec3 &vec) {
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]" << std::endl;
}

void printVec4(const glm::vec4 &vec) {
    std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z  << ", " << vec.w << "]" << std::endl;
}
