#include <IFunctionFactory.h>
#include <boost/python.hpp>
#include <string>

using namespace std;
using namespace boost::python;

struct PyFunctionFactoryPriv {
    object klass;
    string name;
};

template <typename T> class PyFunctionFactory : public IFunctionFactory
{
    public:
        PyFunctionFactory(boost::python::object klass);
        ~PyFunctionFactory();
        virtual T* createFunction() const;
        virtual std::string getFunctionName();
    private:
        PyFunctionFactoryPriv *priv;
};

template <typename T> PyFunctionFactory<T>::PyFunctionFactory(boost::python::object klass) {
        priv = new PyFunctionFactoryPriv();
        priv->klass = klass;
        priv->name = extract<string>(priv->klass.attr("__name__"));
}

template <typename T> PyFunctionFactory<T>::~PyFunctionFactory() {
        delete priv;
}

template <typename T> T *PyFunctionFactory<T>::createFunction() const {
        try {
                object function = priv->klass();
                auto_ptr<T> apf = extract<auto_ptr<T> >(function);
                T* f = apf.get();
                apf.release();
                return f;
        } catch(error_already_set const &) {
                PyErr_Print();
                abort();
        }
        return 0;
}

template <typename T> string PyFunctionFactory<T>::getFunctionName() {
        return priv->name;
}
