#pragma once
#include <vector>

// =============================================================================
//  Patron Object Pool
//  Reserva TODOS los objetos una sola vez al iniciar el juego. Durante la
//  partida nunca se crean ni se borran objetos:
//    - "crear"     = tomar uno inactivo con acquire()
//    - "destruir"  = marcarlo inactivo con setActive(false)
//  Asi la memoria no crece ni se fragmenta (RNF-01).
//
//  Se recorre con un for normal:
//      for (int i = 0; i < pool.size(); i++)
//          if (pool.get(i).isEntityActive()) ...
//
//  T debe derivar de Entity (necesita isEntityActive y setActive).
// =============================================================================
template <typename T>
class ObjectPool
{
public:
    /// Crea 'capacity' objetos inactivos. Es la unica reserva de memoria.
    ObjectPool(int capacity)
        : objects(static_cast<unsigned int>(capacity))
    {
    }

    /// Devuelve un objeto libre o nullptr si todos estan en uso.
    T* acquire()
    {
        for (int i = 0; i < size(); i++)
        {
            if (!get(i).isEntityActive())
                return &get(i);
        }
        return nullptr;
    }

    /// Cantidad total de objetos (activos + libres).
    int size() const
    {
        return static_cast<int>(objects.size());
    }

    /// Acceso al objeto de la posicion i.
    T& get(int i)
    {
        return objects[static_cast<unsigned int>(i)];
    }

    const T& get(int i) const
    {
        return objects[static_cast<unsigned int>(i)];
    }

    /// Cantidad de objetos en uso.
    int activeCount() const
    {
        int count = 0;
        for (int i = 0; i < size(); i++)
        {
            if (get(i).isEntityActive())
                count++;
        }
        return count;
    }

    /// Devuelve todos los objetos al pool (al reiniciar la partida).
    void releaseAll()
    {
        for (int i = 0; i < size(); i++)
            get(i).setActive(false);
    }

private:
    std::vector<T> objects;
};
