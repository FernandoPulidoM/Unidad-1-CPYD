#include <mutex>        // Para proteger la impresión en consola
std::mutex print_mutex; // Mutex global para proteger std::cout

#include <iostream>           // Para imprimir en consola
#include <string>             // Para usar std::string
#include <thread>             // Para usar hilos (threads)
#include <mutex>              // Para usar mutex (exclusión mutua)
#include <condition_variable> // (No se usa, pero se deja por claridad)
#include <memory>             // Para punteros inteligentes (no se usa aquí)
#include <chrono>             // Para temporizadores y sleep

// Estructura que representa un tenedor
struct Fork
{
    std::mutex mtx; // Mutex para controlar el acceso al tenedor
};

// Clase que representa a un filósofo
class Philosopher
{
    std::string name; // Nombre del filósofo
    Fork &leftFork;   // Referencia al tenedor izquierdo
    Fork &rightFork;  // Referencia al tenedor derecho

public:
    // Constructor: recibe el nombre y referencias a los tenedores
    Philosopher(const std::string &n, Fork &l, Fork &r) : name(n), leftFork(l), rightFork(r) {}

    // Método que simula el acto de comer del filósofo
    void dine()
    {
        for (int i = 0; i < 3; ++i) // Cada filósofo come 3 veces
        {
            // Toma ambos tenedores usando scoped_lock para evitar deadlock
            std::scoped_lock lock(leftFork.mtx, rightFork.mtx);
            {
                std::lock_guard<std::mutex> print_lock(print_mutex);
                std::cout << name << " está comiendo (ronda " << (i + 1) << ")..." << std::endl; // Imprime que está comiendo
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simula el tiempo de comer
            {
                std::lock_guard<std::mutex> print_lock(print_mutex);
                std::cout << name << " terminó de comer (ronda " << (i + 1) << ")" << std::endl; // Imprime que terminó de comer
            }
            // Al salir del bloque, los mutex se liberan automáticamente
        }
    }
};

int main()
{
    // Se crean 5 tenedores
    Fork fork1, fork2, fork3, fork4, fork5;
    // Se crean 5 filósofos, cada uno con su tenedor izquierdo y derecho
    Philosopher beethoven("Beethoven", fork1, fork2);     // Beethoven: tenedor 1 (izq), 2 (der)
    Philosopher tchaikovsky("Tchaikovsky", fork2, fork3); // Tchaikovsky: tenedor 2 (izq), 3 (der)
    Philosopher mozart("Mozart", fork3, fork4);           // Mozart: tenedor 3 (izq), 4 (der)
    Philosopher chopin("Chopin", fork4, fork5);           // Chopin: tenedor 4 (izq), 5 (der)
    Philosopher debussy("Debussy", fork5, fork1);         // Debussy: tenedor 5 (izq), 1 (der)

    // Se crean los hilos para cada filósofo, ejecutando su método dine
    std::thread t1(&Philosopher::dine, &beethoven);
    std::thread t2(&Philosopher::dine, &tchaikovsky);
    std::thread t3(&Philosopher::dine, &mozart);
    std::thread t4(&Philosopher::dine, &chopin);
    std::thread t5(&Philosopher::dine, &debussy);

    // Se espera a que todos los hilos terminen
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    return 0;
}
