#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "board.hpp"
#include "player.hpp"

namespace py = pybind11;

PYBIND11_MODULE(Chain_reaction, m) {
    // Cell class
    py::class_<Cell>(m, "Cell")
        .def(py::init<>())
        .def(py::init<int, char>())
        .def("get_orb_count", &Cell::get_orb_count)
        .def("get_color", &Cell::get_color)
        .def("set_orb_count", &Cell::set_orb_count)
        .def("set_color", &Cell::set_color);

    // Board class
    py::class_<Board>(m, "Board")
        .def(py::init<int, int>())
        .def("insert_orb", &Board::insert_orb, py::arg("row"), py::arg("col"), py::arg("color"), py::arg("force") = false)
        .def("get_valid_moves", &Board::get_valid_moves)
        .def("get_score", &Board::get_score)
        .def("is_game_over", &Board::is_game_over)
        .def("get_orb_count", &Board::get_orb_count)
        .def("get_color", &Board::get_color)
        .def("get_rows", &Board::get_rows)
        .def("get_cols", &Board::get_cols)
        .def("get_critical_mass", &Board::get_critical_mass);

    // Human player
    py::class_<Human>(m, "Human")
        .def(py::init<char>())
        .def("make_move", &Human::make_move);

    // AI player
    py::class_<AI>(m, "AI")
        .def(py::init<char, int>())
        .def("make_move", &AI::make_move, py::arg("board"), py::arg("row") = 0, py::arg("col") = 0);
}
