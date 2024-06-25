#include <pybind11/pybind11.h>
#include "multivesc/Manager.hh"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace py = pybind11;

PYBIND11_MODULE(pymultivesc, m) {
    m.doc() = R"pbdoc(
        python wrapper for multivesc
        -----------------------

        .. currentmodule:: pymulivesc

        .. autosummary::
           :toctree: _generate
    )pbdoc";

    py::class_<multivesc::Manager>(m, "Manager")
     .def("open_can", &multivesc::Manager::openCan)
     .def("start", &multivesc::Manager::start)
     .def("stop", &multivesc::Manager::stop)
     .def("set_verbose", &multivesc::Manager::setVerbose)
     .def("verbose", &multivesc::Manager::verbose)
     .def("get_motor_by_id", &multivesc::Manager::getMotor)
     .def("get_motor_by_name", &multivesc::Manager::getMotorByName)
    ;

    py::class_<multivesc::Motor>(m, "Motor")
    .def("set_name", &multivesc::Motor::setName)
    .def("name", &multivesc::Motor::name)
    .def("set_rpm", &multivesc::Motor::setRPM)
    .def("set_current", &multivesc::Motor::setCurrent)
    .def("set_duty", &multivesc::Motor::setDuty)
    ;


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
