#ifndef PYDOMAINWRAPPER_H
#define PYDOMAINWRAPPER_H

#include <Domain.h>
#include <boost/python.hpp>

void wrapDomain()
{
    class_<Domain >("Domain",init<std::string, Domain::DOMAINTYPE>())
            .def("getType",&Domain::getType)
            .def("getName",&Domain::getName)
            .def("setSuperDomain", &Domain::setSuperDomain)
            .def("getParameter", &Domain::getParameter,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("contains", &Domain::contains)
            .def("setParameter", &Domain::setParameter)
            ;
}

#endif // PYDOMAINWRAPPER_H
