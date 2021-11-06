#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
    return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for (auto neighbor : current_node->neighbors) {
        neighbor->parent = current_node;
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
        neighbor->h_value = CalculateHValue(neighbor);
        if (!neighbor->visited) {
            open_list.emplace_back(neighbor);
            neighbor->visited = true;
        }
    }
}

bool compare(RouteModel::Node *a, RouteModel::Node *b) {
    float f1 = a->g_value + a->h_value;
    float f2 = b->g_value + b->h_value;

    return f1 > f2;
}

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort(open_list.begin(), open_list.end(), compare);
    RouteModel::Node* next_node = open_list.back();
    open_list.pop_back();
    return next_node;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    while (current_node != nullptr) {
        path_found.insert(path_found.begin(), *current_node);
        if (current_node->parent != nullptr) {
            distance += current_node->distance(*(current_node->parent));
        }
        current_node = current_node->parent;
    }

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;
}

void RoutePlanner::AStarSearch() {
    start_node->visited = true;
    AddNeighbors(start_node);

    RouteModel::Node *current_node = nullptr;

    while (open_list.size() != 0) {
        current_node = NextNode();

        if (current_node->distance(*end_node) == 0) {
            m_Model.path = ConstructFinalPath(current_node);
        }
        AddNeighbors(current_node);
    }
}