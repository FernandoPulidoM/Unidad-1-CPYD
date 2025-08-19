
#include <iostream> // Para imprimir en consola
#include <thread>   // Para usar hilos (threads)
#include <vector>   // Para usar std::vector
#include <random>   // Para generar números aleatorios
#include <mutex>    // Para usar mutex y proteger la impresión

std::mutex cout_mutex; // Mutex global para proteger std::cout

// Función que suma 100 números aleatorios entre 1 y 1000 en un hilo
void sum_random_numbers(int thread_id, int &result)
{
    std::random_device rd;                        // Fuente de entropía para el generador aleatorio
    std::mt19937 gen(rd());                       // Generador de números aleatorios
    std::uniform_int_distribution<> dis(1, 1000); // Distribución uniforme entre 1 y 1000
    int sum = 0;                                  // Acumulador de la suma
    for (int i = 0; i < 100; ++i)                 // Repite 100 veces
    {
        sum += dis(gen); // Suma un número aleatorio a sum
    }
    result = sum;                                                           // Guarda el resultado en la referencia
    std::lock_guard<std::mutex> lock(cout_mutex);                           // Protege la impresión en consola
    std::cout << "Thread " << thread_id << ": suma = " << sum << std::endl; // Imprime el resultado del hilo
}

int main()
{
    const int num_threads = 10;               // Número de hilos a crear
    std::vector<std::thread> threads;         // Vector para guardar los hilos
    std::vector<int> results(num_threads, 0); // Vector para guardar los resultados de cada hilo

    // Crea y lanza los hilos
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(sum_random_numbers, i, std::ref(results[i])); // Lanza un hilo y guarda su resultado por referencia
    }
    // Espera a que todos los hilos terminen
    for (auto &t : threads)
    {
        t.join(); // Espera a que el hilo termine
    }

    int max_sum = results[0];             // Inicializa la suma máxima con el primer resultado
    int max_thread = 0;                   // Índice del hilo con la suma máxima
    for (int i = 1; i < num_threads; ++i) // Recorre los resultados
    {
        if (results[i] > max_sum) // Si encuentra una suma mayor
        {
            max_sum = results[i]; // Actualiza la suma máxima
            max_thread = i;       // Actualiza el índice del hilo
        }
    }
    // Imprime el hilo con la suma más alta
    std::cout << "\nEl thread con la puntuación más alta es el " << max_thread << " con suma = " << max_sum << std::endl;
    return 0; // Fin del programa
}
