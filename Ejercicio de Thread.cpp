#include <iostream>  // Para imprimir en consola
#include <thread>    // Para usar hilos
#include <mutex>     // Para proteger la impresion
#include <vector>    // Para usar vectores
#include <random>    // Para generar numeros aleatorios
#include <algorithm> // Para std::max_element
#include <iterator>  // Para std::distance

std::mutex cout_mutex;                        // Mutex global para proteger std::cout
std::random_device rd;                        // Fuente de entropia
std::mt19937 gen(rd());                       // Generador de numeros aleatorios
std::uniform_int_distribution<> dis(1, 1000); // Distribucion uniforme entre 1 y 1000

class num_thread // Clase que representa un hilo que suma números aleatorios
{
public:
    int id;        // Identificador del hilo
    int result;    // Resultado de la suma
    std::thread t; // Hilo asociado al objeto

    num_thread(int thread_id) : id(thread_id), result(0) {} // Constructor que inicializa el id y el resultado

    // Función que suma 100 números aleatorios entre 1 y 1000
    void sum_random_numbers()
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
        t = std::thread(&num_thread::sum_random_numbers, this); // Inicia el hilo, pasando la función miembro y el objeto actual
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
    int num_threads = 10;

    std::vector<num_thread> thread_objs; // Vector para guardar los objetos de los hilos
    thread_objs.reserve(num_threads);    // Reservar espacio para los objetos

    for (int i = 0; i < num_threads; i++) // Crear los objetos de los hilos
    {
        thread_objs.emplace_back(i + 1); // Emplace back para evitar copias innecesarias
    }

    // Iniciar los hilos
    for (int i = 0; i < num_threads; i++)
    {
        thread_objs[i].start();
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_threads; i++)
    {
        thread_objs[i].join();
    }

    // Encontrar el mayor resultado con un for rudimentario
    int max_sum = thread_objs[0].result; // asumimos que el primero es el mayor
    int max_thread = thread_objs[0].id;

    // Iterar sobre los resultados para encontrar el máximo
    for (int i = 1; i < num_threads; i++)
    {
        if (thread_objs[i].result > max_sum) // si encontramos un mayor, actualizamos
        {
            max_sum = thread_objs[i].result; // nuevo maximo
            max_thread = thread_objs[i].id;  // id del hilo con el nuevo maximo
        }
    }

    std::cout << "\nEl thread con la puntuacion más alta es el "
              << max_thread << " con suma = " << max_sum << std::endl;

    return 0;
}
