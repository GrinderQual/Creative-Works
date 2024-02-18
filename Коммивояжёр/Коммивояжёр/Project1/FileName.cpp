#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

// Если константа PI не определена, определяем её
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int INF = std::numeric_limits<int>::max(); // Бесконечность для представления отсутствия пути
const float NODE_RADIUS = 20.0f; // Радиус отображения узла
const int WINDOW_WIDTH = 800; // Ширина окна
const int WINDOW_HEIGHT = 600; // Высота окна

std::vector<sf::CircleShape> nodes; // Вектор для хранения узлов графа
std::vector<std::vector<int>> graph; // Матрица смежности для представления графа
std::vector<bool> visited; // Вектор для отметки посещенных узлов
std::vector<int> min_path; // Вектор для хранения минимального пути
int min_cost = INF; // Минимальная стоимость пути
sf::Font font; // Шрифт для отображения текста

// Функция для проверки, находится ли курсор мыши над узлом
bool isMouseOverNode(sf::Vector2f mousePosition, sf::Vector2f nodePosition) {
    float dx = mousePosition.x - nodePosition.x;
    float dy = mousePosition.y - nodePosition.y;
    return std::sqrt(dx * dx + dy * dy) < NODE_RADIUS;
}

// Функция для расчета нижней границы стоимости пути через узел
int calculateLowerBound(int node, std::vector<int>& path) {
    int bound = 0;

    // Проходим по всем узлам
    for (int i = 0; i < graph.size(); ++i) {
        if (!visited[i]) { // Если узел не посещен
            int min_edge = INF;
            // Ищем минимальное ребро
            for (int j = 0; j < graph.size(); ++j) {
                if (graph[i][j] < min_edge && i != j) {
                    min_edge = graph[i][j];
                }
            }
            bound += min_edge; // Добавляем вес минимального ребра к границе
        }
    }

    bound += graph[node][0]; // Добавляем вес ребра от текущего узла до начального
    return bound;
}

// Рекурсивная функция для решения задачи коммивояжёра методом ветвей и границ
void tsp(int node, int depth, int cost, std::vector<int>& path) {
    if (depth == graph.size()) { // Если все узлы посещены
        if (graph[node][0] != INF) { // Если существует путь обратно в начальный узел
            cost += graph[node][0]; // Добавляем стоимость пути обратно
            if (cost < min_cost) { // Если текущий путь дешевле найденного ранее
                min_cost = cost; // Обновляем минимальную стоимость
                min_path = path; // Сохраняем текущий путь как минимальный
            }
        }
        return;
    }

    // Проходим по всем узлам
    for (int i = 0; i < graph.size(); ++i) {
        if (!visited[i] && graph[node][i] != INF) { // Если узел не посещен и существует путь
            visited[i] = true; // Отмечаем узел как посещенный
            path.push_back(i); // Добавляем узел в текущий путь
            int new_cost = cost + graph[node][i]; // Увеличиваем текущую стоимость пути
            int lower_bound = calculateLowerBound(i, path); // Рассчитываем нижнюю границу

            // Если текущая стоимость плюс нижняя граница меньше минимальной стоимости
            if (new_cost + lower_bound < min_cost) {
                tsp(i, depth + 1, new_cost, path); // Продолжаем рекурсивный поиск
            }

            visited[i] = false; // Снимаем отметку о посещении
            path.pop_back(); // Удаляем узел из текущего пути
        }
    }
}

// Функция для отрисовки узла на экране
void drawNode(sf::RenderWindow& window, float x, float y, int nodeIndex) {
    sf::CircleShape node(NODE_RADIUS); // Создаем форму узла
    node.setPosition(x - NODE_RADIUS, y - NODE_RADIUS); // Устанавливаем позицию узла
    node.setFillColor(sf::Color::Green); // Устанавливаем цвет узла
    window.draw(node); // Рисуем узел на экране

    sf::Text label(std::to_string(nodeIndex), font); // Создаем текстовую метку для узла
    label.setCharacterSize(15); // Устанавливаем размер шрифта метки
    label.setFillColor(sf::Color::White); // Устанавливаем цвет текста метки
    label.setPosition(x - NODE_RADIUS / 2, y - NODE_RADIUS / 2); // Позиционируем метку
    window.draw(label); // Рисуем метку на экране
}

// Функция для отрисовки ребра между двумя узлами
void drawEdge(sf::RenderWindow& window, sf::Vector2f from, sf::Vector2f to, sf::Color color = sf::Color::White) {
    sf::VertexArray line(sf::LinesStrip, 2); // Создаем линию из двух точек
    line[0].position = from + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Начальная точка линии
    line[1].position = to + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Конечная точка линии
    line[0].color = color; // Устанавливаем цвет начала линии
    line[1].color = color; // Устанавливаем цвет конца линии
    window.draw(line); // Рисуем линию на экране
}

// Функция для генерации случайного графа
void generateRandomGraph(int num_nodes) {
    srand(static_cast<unsigned int>(time(nullptr))); // Инициализация генератора случайных чисел
    graph.resize(num_nodes, std::vector<int>(num_nodes, INF)); // Инициализация матрицы смежности
    visited.resize(num_nodes, false); // Инициализация вектора посещенных узлов

    // Заполнение матрицы смежности случайными весами рёбер
    for (int i = 0; i < num_nodes; ++i) {
        for (int j = i + 1; j < num_nodes; ++j) {
            int weight = rand() % 100 + 1; // Случайный вес ребра
            graph[i][j] = weight; // Задаем вес ребра от i к j
            graph[j][i] = weight; // Задаем вес ребра от j к i
        }
    }
}

// Функция для решения задачи коммивояжёра
void solveTSP() {
    visited[0] = true; // Отмечаем начальный узел как посещенный
    std::vector<int> path{ 0 }; // Инициализируем путь начальным узлом
    tsp(0, 1, 0, path); // Запускаем решение задачи коммивояжёра
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "TSP Solver"); // Создаем окно приложения
    window.setFramerateLimit(60); // Устанавливаем лимит кадров в секунду

    // Загрузка шрифта
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
        return 1; // В случае ошибки завершаем программу с кодом 1
    }

    int num_nodes = 6; // Количество узлов в графе
    generateRandomGraph(num_nodes); // Генерируем случайный граф

    // Располагаем узлы графа по окружности
    for (int i = 0; i < num_nodes; ++i) {
        float angle = 2 * M_PI * i / num_nodes; // Вычисляем угол для каждого узла
        float x = WINDOW_WIDTH / 2 + cos(angle) * 200; // Вычисляем x координату узла
        float y = WINDOW_HEIGHT / 2 + sin(angle) * 200; // Вычисляем y координату узла
        nodes.emplace_back(sf::CircleShape(NODE_RADIUS)); // Создаем форму узла
        nodes.back().setPosition(x - NODE_RADIUS, y - NODE_RADIUS); // Устанавливаем позицию узла
        nodes.back().setFillColor(sf::Color::Green); // Устанавливаем цвет узла
    }

    solveTSP(); // Решаем задачу коммивояжёра

    int draggedNodeIndex = -1; // Индекс перетаскиваемого узла

    // Основной цикл приложения
    while (window.isOpen()) {
        sf::Event event;
        // Обработка событий
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // Закрытие окна

            // Нажатие кнопки мыши
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window)); // Получаем позицию курсора

                // Проверяем, наведен ли курсор на узел
                for (size_t i = 0; i < nodes.size(); ++i) {
                    if (isMouseOverNode(mousePosition, nodes[i].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS))) {
                        draggedNodeIndex = i; // Запоминаем индекс перетаскиваемого узла
                        break;
                    }
                }

                // Если курсор не наведен на узел, добавляем новый узел
                if (draggedNodeIndex == -1) {
                    sf::CircleShape newNode(NODE_RADIUS); // Создаем форму нового узла
                    newNode.setPosition(mousePosition.x - NODE_RADIUS, mousePosition.y - NODE_RADIUS); // Устанавливаем позицию нового узла
                    newNode.setFillColor(sf::Color::Green); // Устанавливаем цвет нового узла
                    nodes.push_back(newNode); // Добавляем новый узел в вектор узлов
                    int newSize = nodes.size(); // Получаем новый размер вектора узлов
                    // Обновляем матрицу смежности и вектор посещенных узлов
                    for (auto& row : graph) {
                        row.push_back(INF);
                    }
                    graph.push_back(std::vector<int>(newSize, INF));
                    visited.push_back(false);
                }
            }

            // Отпускание кнопки мыши
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && draggedNodeIndex != -1) {
                    draggedNodeIndex = -1; // Сбрасываем индекс перетаскиваемого узла
                }
            }

            // Перемещение мыши
            if (event.type == sf::Event::MouseMoved) {
                // Если узел перетаскивается
                if (draggedNodeIndex != -1) {
                    sf::Vector2f newPosition = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)); // Получаем новую позицию курсора
                    nodes[draggedNodeIndex].setPosition(newPosition.x - NODE_RADIUS, newPosition.y - NODE_RADIUS); // Перемещаем узел в новую позицию
                }
            }
        }

        window.clear(sf::Color::Black); // Очищаем окно

        // Отрисовка рёбер графа
        for (int i = 0; i < graph.size(); ++i) {
            for (int j = i + 1; j < graph[i].size(); ++j) {
                if (graph[i][j] != INF) { // Если ребро существует
                    sf::Vector2f nodePositionI = nodes[i].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Позиция первого узла ребра
                    sf::Vector2f nodePositionJ = nodes[j].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Позиция второго узла ребра
                    drawEdge(window, nodePositionI, nodePositionJ); // Отрисовка ребра
                }
            }
        }

        // Отрисовка узлов графа
        for (auto& node : nodes) {
            window.draw(node); // Отрисовка каждого узла
        }

        // Отрисовка минимального пути
        if (!min_path.empty()) {
            for (size_t i = 0; i < min_path.size() - 1; ++i) {
                sf::Vector2f from = nodes[min_path[i]].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Позиция начального узла пути
                sf::Vector2f to = nodes[min_path[i + 1]].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS); // Позиция конечного узла пути
                drawEdge(window, from, to, sf::Color::Red); // Отрисовка ребра пути красным цветом
            }
            // Замыкаем путь, соединяя последний и первый узлы
            drawEdge(window, nodes[min_path.back()].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS), nodes[min_path[0]].getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS), sf::Color::Red);
        }

        window.display(); // Отображение содержимого окна
    }

    return 0; // Завершение программы
}
