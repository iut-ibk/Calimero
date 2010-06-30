#include <IFunctionFactory.h>
#include <boost/python.hpp>
#include <string>
#include <Exception.h>
#include <PyEnv.h>
#include <PyIFunctionWrapper.h>
#include <QMutex>
#include <QMutexLocker>

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
        QMutex *mutex;
};

template <typename T> PyFunctionFactory<T>::PyFunctionFactory(boost::python::object klass) {
        priv = new PyFunctionFactoryPriv();
        priv->klass = klass;
        priv->name = extract<string>(priv->klass.attr("__name__"));
        mutex = new QMutex();
}

template <typename T> PyFunctionFactory<T>::~PyFunctionFactory() {
        delete priv;
        delete mutex;
}

template <typename T> T* PyFunctionFactory<T>::createFunction() const {
        ScopedGILRelease scoped;
        try {
            object function = priv->klass();
            shared_ptr<T> apf = extract<shared_ptr<T> >(function);
            IFunction* f = apf.get();
            FunctionWrapper *fw = static_cast<FunctionWrapper*>(f);
            //apf.release();
            fw->self = function;
            return static_cast<T*>(f);
        } catch(error_already_set const &) {
            handle_python_exception("Could not create an instance of function \"" + priv->name + "\"");
        }
        return 0;
}

template <typename T> string PyFunctionFactory<T>::getFunctionName() {
        return priv->name;
}
