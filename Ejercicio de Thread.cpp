#include <iostream>  // Para imprimir en consola
#include <thread>    // Para usar hilos
#include <mutex>     // Para proteger la impresion
#include <vector>    //Para usar vectores
#include <random>    // Para generar numeros aleatorios
#include <algorithm> // Para std::max_element
#include <iterator>  // Para std::distance

std::mutex cout_mutex;                        // Mutex global para proteger std::cout
std::random_device rd;                        // Fuente de entropia para el generador aleatorio
std::mt19937 gen(rd());                       // Generador de numeros aleatorios
std::uniform_int_distribution<> dis(1, 1000); // Distribucion uniforme entre 1 y 1000

// Función que suma 100 números aleatorios entre 1 y 1000 en un hilo
void sum_random_numbers(int thread_id, int &result)
{
    int sum = 0; // guarda la suma
    for (int i = 0; i < 100; i++)
    {
        sum += dis(gen); // suma numero aleatorio del rango dado a sum
    }

    result = sum; // guarda en el parametro

    std::lock_guard<std::mutex> lock(cout_mutex); // Asegura que la impresion no se sobreescriba
    std::cout << "Thread" << thread_id << ": suma = " << sum << std::endl;
}

int main()
{
    int num_threads = 10;                     // numero de hilos creados
    std::vector<std::thread> threads;         // vector para guardar hilos
    std::vector<int> results(num_threads, 0); // vector para guardar los resultados de cada hilo

    // Crea y lanza los hilos
    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(sum_random_numbers, i, std::ref(results[i])); // Lanza un hilo y guarda su resultado por referencia
    }

    // Espera a que todos los hilos terminen
    for (auto &t : threads)
    {
        t.join(); // Espera a que el hilo termine
    }

    // Usar std::max_element para encontrar el valor máximo y su índice
    auto max_it = std::max_element(results.begin(), results.end()); // Encuentra el iterador al elemento máximo
    int max_sum = *max_it;                                          // Obtiene el valor máximo
    int max_thread = std::distance(results.begin(), max_it);        // Obtiene el índice del elemento máximo

    // imprime el hilo con la suma mas alta
    std::cout << "\nEl thread con la puntacion más alta es el " << max_thread << " con suma = " << max_sum << std::endl;
    return 0; // fin del programa
}
