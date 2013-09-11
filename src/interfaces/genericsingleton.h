/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef GENERICSINGLETON_H
#define GENERICSINGLETON_H

#include <QMutex>

template <typename T>
class GenericSingleton
{
protected:
    GenericSingleton(){}
    ~GenericSingleton(){}

private:
    GenericSingleton(const GenericSingleton &){}

public:
    static T *instance() {
        if(!_instance) {
            _mutex.lock();
            if(!_instance)
                _instance = new T;
            _mutex.unlock();
        }
        return _instance;
    }

    static void deleteInstance () {
        _mutex.lock();
        delete _instance;
        _instance = nullptr;
        _mutex.unlock();
    }
protected:
    static QMutex _mutex;

private:
    static T *_instance;
    T& operator= (const T&){}
};


template <typename T>
T *GenericSingleton<T>::_instance = nullptr;
template <typename T>
QMutex GenericSingleton<T>::_mutex;

#endif // GENERICSINGLETON_H
