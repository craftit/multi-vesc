#include <pybind11/pybind11.h>
#include <pybind11_json/pybind11_json.hpp>
#include "multivesc/Manager.hh"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace py = pybind11;

PYBIND11_MODULE(pymultivesc, m) {
    m.doc() = R"pbdoc(
        python wrapper for multivesc
        -----------------------

        .. currentmodule:: pymultivesc

        .. autosummary::
           :toctree: _generate
    )pbdoc";

    py::class_<multivesc::Manager>(m, "Manager")
      .def(py::init<multivesc::json>())
      .def(py::init<>())
     .def("start", &multivesc::Manager::start)
     .def("stop", &multivesc::Manager::stop)
     .def("configure", &multivesc::Manager::configure)
     .def("set_verbose", &multivesc::Manager::setVerbose)
     .def("verbose", &multivesc::Manager::verbose)
     .def("motor", &multivesc::Manager::getMotor)
     .def("motors", &multivesc::Manager::motors)
    ;

    py::class_<multivesc::Motor, std::shared_ptr<multivesc::Motor>>(m, "Motor")
    .def("name", &multivesc::Motor::name)
    .def("id", &multivesc::Motor::id)
    .def("set_rpm", &multivesc::Motor::setRPM)
    .def("set_current", &multivesc::Motor::setCurrent)
    .def("set_duty", &multivesc::Motor::setDuty)
    .def("rpm", &multivesc::Motor::rpm)
    .def("current", &multivesc::Motor::current)
    .def("duty", &multivesc::Motor::duty)
    .def("set_min_rpm", &multivesc::Motor::setMinRPM)
    .def("set_max_rpm_acceleration", &multivesc::Motor::setMaxRPMAcceleration)
    .def("amp_hours", &multivesc::Motor::ampHours)
    .def("amp_hours_charged", &multivesc::Motor::ampHoursCharged)
    .def("watt_hours", &multivesc::Motor::wattHours)
    .def("watt_hours_charged", &multivesc::Motor::wattHoursCharged)
    .def("temp_fet", &multivesc::Motor::tempFet)
    .def("temp_motor", &multivesc::Motor::tempMotor)
    .def("vin", &multivesc::Motor::vIn)
    .def("tachometer", &multivesc::Motor::tachometer)
    ;


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
