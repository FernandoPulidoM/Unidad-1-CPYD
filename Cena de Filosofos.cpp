#include <memory>             // Para usar std::shared_ptr
#include <string>             // usar std::string
#include <thread>             // Para usar std::thread
#include <chrono>             // Para usar std::chrono::seconds
#include <iostream>           // imprimir en consola
#include <mutex>              // Para usar std::mutex
#include <condition_variable> //Para usar std::condition_variable

std::mutex coutMutex; // Mutex para proteger la salida a consola

// Estructura que representa un tenedor
// Cada tenedor tiene un estado (si está tomado o no), el nombre del filósofo que lo está usando,
// un mutex para proteger el acceso al tenedor y una variable de condición para esperar a que el tenedor esté disponible.
struct Fork
{
    bool isTaken = false;          // Indica si el tenedor está en uso
    std::string_view philosopher;  // Nombre del filósofo que está usando el tenedor
    std::mutex forkMutex;          // Mutex para proteger el acceso al tenedor, nadie más puede usarlo mientras un filósofo lo tiene
    std::condition_variable taken; // Variable de condición para esperar a que el tenedor esté disponible

    // función para tomar el tenedor
    void takeFork(std::string_view philosopherName, std::string_view hand)
    {
        {
            std::unique_lock lock(forkMutex); // Usar unique_lock para poder usar condition_variable
            taken.wait(lock, [&]()            // Esperar hasta que el tenedor esté disponible
                       {
                {
                std::lock_guard<std::mutex> lockCout(coutMutex); // Proteger la salida a consola, no se interrumpa la salida de otros filósofos
                std::cout << philosopherName << " is waiting for " << hand << " hand fork" << std::endl; 
                }
                return !isTaken; });                  // Esperar hasta que isTaken sea false

            {
                std::lock_guard<std::mutex> lockCout(coutMutex); // Proteger la salida a consola
                std::cout << philosopherName << " is taking " << hand << " hand fork" << std::endl;
            }

            isTaken = true;                // Marcar el tenedor como tomado
            philosopher = philosopherName; // Asignar el nombre del filósofo que está usando el tenedor
        }
    }

    // función para liberar el tenedor
    void releaseFork(std::string_view hand)
    {
        {
            std::lock_guard<std::mutex> lockCout(coutMutex); // Proteger la salida a consola
            std::cout << philosopher << " is releasing " << hand << " hand fork" << std::endl;
        }

        isTaken = false;        // Marcar el tenedor como no tomado
        this->philosopher = ""; // Limpiar el nombre del filósofo que estaba usando el tenedor
        taken.notify_all();     // Notificar a todos los filósofos que están esperando por el tenedor
    }
};

// Clase que representa a un filósofo
class Philosopher
{
    std::string name;                // Nombre del filósofo
    std::shared_ptr<Fork> leftFork;  // Tenedor a la izquierda
    std::shared_ptr<Fork> rightFork; // Tenedor a la derecha

public:
    Philosopher(std::string name) : name(name) {} // Constructor que inicializa el nombre del filósofo

    std::shared_ptr<Fork> RightFork() const // Función para obtener el tenedor a la derecha
    {
        return rightFork;
    }

    std::shared_ptr<Fork> &RightFork() // Función para obtener el tenedor a la derecha (no constante)
    {
        return rightFork;
    }

    std::shared_ptr<Fork> LeftFork() const // Función para obtener el tenedor a la izquierda
    {
        return leftFork;
    }
    std::shared_ptr<Fork> &LeftFork() // Función para obtener el tenedor a la izquierda (no constante)
    {
        return leftFork;
    }

    // Función que simula el acto de comer
    // El filósofo come 3 veces, tomando primero el tenedor de la izquierda y luego el de la derecha
    // Después de comer, libera los tenedores y espera un poco antes de intentar comer de nuevo
    // Esto simula el comportamiento de los filósofos comiendo, evitando el deadlock al tomar los tenedores en orden
    // y liberándolos después de comer.
    void eat()
    {
        int stomach = 3; // numero de veces que comera

        // Ciclo que simula el acto de comer
        //  El filósofo come hasta que su estómago esté lleno (3 veces)
        //  Toma primero el tenedor de la izquierda y luego el de la derecha
        while (stomach > 0)
        {
            auto first = (leftFork < rightFork) ? leftFork : rightFork;  // Determinar cuál tenedor tomar primero
            auto second = (leftFork < rightFork) ? rightFork : leftFork; // Determinar cuál tenedor tomar segundo

            first->takeFork(name, "first");   // Tomar el primer tenedor (izquierda o derecha)
            second->takeFork(name, "second"); // Tomar el segundo tenedor (derecha o izquierda)

            {
                std::lock_guard<std::mutex> lockCout(coutMutex); // Proteger la salida a consola
                std::cout << name << " started eating" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(2)); // Simular el tiempo que tarda en comer

            --stomach; // Disminuir el contador de estómago para que no coman infinitamente

            {
                std::lock_guard<std::mutex> lockCout(coutMutex);                               // Proteger la salida a consola
                std::cout << name << " done eating: " << stomach << "/3 missing" << std::endl; // Imprimir el número de veces que le falta comer
                std::cout << std::endl;
            }

            second->releaseFork("second"); // Liberar el segundo tenedor (derecha o izquierda)
            first->releaseFork("first");   // Liberar el primer tenedor (izquierda o derecha)

            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // dejar que otros intenten
        }

        {
            std::lock_guard<std::mutex> lockCout(coutMutex); // Proteger la salida a consola
            std::cout << name << " finished eating" << std::endl;
            std::cout << std::endl;
        }
    }
};

// Función principal
// Crea los tenedores y los filósofos, asigna los tenedores a los filósofos y crea hilos para cada filósofo
// Luego espera a que todos los hilos terminen antes de finalizar el programa
// Esto simula el problema de los filósofos comiendo, donde cada filósofo necesita dos tenedores para comer
// y deben coordinarse para evitar el deadlock.
int main(void)
{
    // Crear los tenedores (forks)
    std::shared_ptr<Fork> fork1 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork2 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork3 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork4 = std::make_shared<Fork>();
    std::shared_ptr<Fork> fork5 = std::make_shared<Fork>();

    // Crear los filósofos y asignarles los tenedores
    std::shared_ptr<Philosopher> beethoven = std::make_shared<Philosopher>("Beethoven");
    beethoven->LeftFork() = fork1;
    beethoven->RightFork() = fork2;

    std::shared_ptr<Philosopher> tchaikovsky = std::make_shared<Philosopher>("Tchaikovsky");
    tchaikovsky->LeftFork() = fork2;
    tchaikovsky->RightFork() = fork3;

    std::shared_ptr<Philosopher> mozart = std::make_shared<Philosopher>("Mozart");
    mozart->LeftFork() = fork3;
    mozart->RightFork() = fork4;

    std::shared_ptr<Philosopher> ferdinand = std::make_shared<Philosopher>("Ferdinand");
    ferdinand->LeftFork() = fork4;
    ferdinand->RightFork() = fork5;

    std::shared_ptr<Philosopher> chopin = std::make_shared<Philosopher>("Chopin");
    chopin->LeftFork() = fork5;
    chopin->RightFork() = fork1;

    // Crear los hilos para cada filósofo y activar la función eat
    // Cada filósofo intentará comer en su propio hilo, lo que permite que varios filósofos coman al mismo tiempo
    // y evita el deadlock al tomar los tenedores en orden.
    std::cout << "Musicians started dinning..." << std::endl;
    std::thread threadBeethoven([&]()
                                { beethoven->eat(); });
    std::thread threadTchaikovsky([&]()
                                  { tchaikovsky->eat(); });
    std::thread threadMozart([&]()
                             { mozart->eat(); });
    std::thread threadFerdinand([&]()
                                { ferdinand->eat(); });
    std::thread threadChopin([&]()
                             { chopin->eat(); });

    // Esperar a que terminen los hilos
    threadChopin.join();
    threadFerdinand.join();
    threadBeethoven.join();
    threadTchaikovsky.join();
    threadMozart.join();

    std::cout << "They all finished eating." << std::endl;

    return 0;
}
