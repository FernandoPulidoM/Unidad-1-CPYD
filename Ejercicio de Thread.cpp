#include <iostream>  // Para imprimir en consola
#include <thread>    // Para usar hilos
#include <mutex>     // Para proteger la impresion
#include <vector>    // Para usar vectores
#include <random>    // Para generar numeros aleatorios
#include <algorithm> // Para std::sort
#include <iterator>  // Para std::distance

std::mutex cout_mutex;                        // Mutex global para proteger std::cout
std::random_device rd;                        // Fuente de entropia
std::mt19937 gen(rd());                       // Generador de numeros aleatorios
std::uniform_int_distribution<> dis(1, 1000); // Distribucion uniforme entre 1 y 1000

class numThread // Clase que representa un hilo que suma números aleatorios
{
public:
    int id;        // Identificador del hilo
    int result;    // Resultado de la suma
    std::thread t; // Hilo asociado al objeto

    numThread(int idThread) : id(idThread), result(0) {} // Constructor que inicializa el id y el resultado

    // Función que suma 100 números aleatorios entre 1 y 1000
    void sumRandom()
    {
        int sum = 0;                  // guarda la suma
        for (int i = 0; i < 100; i++) // Suma 100 números aleatorios
        {
            sum += dis(gen); // suma numero aleatorio del rango dado a sum
        }
        result = sum; // guarda en el atributo result

        std::lock_guard<std::mutex> lock(cout_mutex); // Asegura que la impresion no se sobreescriba
        std::cout << "Thread " << id << ": suma = " << sum << std::endl;
    }

    // Inicia el hilo
    void start()
    {
        t = std::thread(&numThread::sumRandom, this); // Inicia el hilo, pasando la función miembro y el objeto actual
    }

    // Espera que termine el hilo
    void join()
    {
        if (t.joinable()) // Verifica si el hilo es unible antes de unirse
            t.join();     // Espera a que el hilo termine
    }
};

int main()
{
    int numThreads = 10;

    std::vector<numThread> objThread; // Vector para guardar los objetos de los hilos
    objThread.reserve(numThreads);    // Reservar espacio para los objetos

    for (int i = 0; i < numThreads; i++) // Crear los objetos de los hilos
    {
        objThread.emplace_back(i + 1); // Emplace back para evitar copias innecesarias
    }

    // Iniciar los hilos
    for (int i = 0; i < numThreads; i++)
    {
        objThread[i].start();
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < numThreads; i++)
    {
        objThread[i].join();
    }

    // Ordenar los threads por resultado usando std::sort y tomar el mayor al final
    std::sort(objThread.begin(), objThread.end(), [](const numThread &a, const numThread &b)
              { return a.result < b.result; });
    const numThread &maxThread = objThread.back();
    std::cout << "\nEl thread con la puntuacion más alta es el "
              << maxThread.id << " con suma = " << maxThread.result << std::endl;

    return 0;
}
