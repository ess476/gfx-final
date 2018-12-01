//
// Created by gmarks on 9/30/18.
//

#ifndef PROJECTS_SQUAREGRID_H
#define PROJECTS_SQUAREGRID_H
#include "Shape.h"
#include <functional>

class SquareGrid : public Shape {
 public:
    SquareGrid(int nx, int ny, glm::mat4 trans, glm::vec3 normal);
    SquareGrid(int nx, int ny, glm::mat4 trans, glm::vec3 normal, std::function<glm::vec4(glm::vec4)> grid_f, std::function<glm::vec4(glm::vec4)> norm_f);
    void concatR(SquareGrid other);
    void concatD(SquareGrid other);
    void buildShape();
    std::vector<std::tuple<glm::vec4, glm::vec4>> m_grid;
 protected:
    std::vector<std::tuple<glm::vec4, glm::vec4>> makeGrid();
    int m_col, m_row;
    glm::mat4 m_trans;
    glm::vec3 m_N;
    std::function<glm::vec4(glm::vec4)> m_grid_fn;
    std::function<glm::vec4(glm::vec4)> m_norm_fn;

};

#endif //PROJECTS_SQUAREGRID_H