#include <IFunctionFactory.h>
#include <boost/python.hpp>
#include <string>
#include <PyException.h>
#include <PyEnv.h>
#include <PyIFunctionWrapper.h>

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

template <typename T> T* PyFunctionFactory<T>::createFunction() const {
        try {
                object function = priv->klass();
                auto_ptr<T> apf = extract<auto_ptr<T> >(function);
                IFunction* f = apf.get();
                FunctionWrapper *fw = static_cast<FunctionWrapper*>(f);
                apf.release();
                fw->self = function;
                return static_cast<T*>(f);
        } catch(error_already_set const &) {
            handle_python_exception();
        }
        return 0;
}

template <typename T> string PyFunctionFactory<T>::getFunctionName() {
        return priv->name;
}
