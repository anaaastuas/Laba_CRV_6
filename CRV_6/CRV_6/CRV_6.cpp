#include <coroutine>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <iomanip>
#include <clocale>


// ========== ПУНКТ 1: Простая корутина ==========

struct SimplePromise {
    auto get_return_object() {
        return std::coroutine_handle<SimplePromise>::from_promise(*this);
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

struct SimpleCoroutine {
    using promise_type = SimplePromise;
    std::coroutine_handle<promise_type> handle;

    SimpleCoroutine(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~SimpleCoroutine() { if (handle) handle.destroy(); }

    void resume() { if (handle) handle.resume(); }
};

namespace std {
    template<>
    struct coroutine_traits<SimpleCoroutine, int> {
        using promise_type = SimplePromise;
    };
}

SimpleCoroutine print_number(int number) {
    std::cout << "  Выводимое число: " << number << std::endl;
    std::cout << "  Квадрат числа: " << number * number << std::endl;
    std::cout << "  Куб числа: " << number * number * number << std::endl;
    co_return;
}

void run_task1() {
    std::cout << "=== ПУНКТ 1 ===" << std::endl;
    std::cout << "Пример из документации с моим числом" << std::endl;

    auto coro = print_number(455); 
    coro.resume();
}

// ========== ПУНКТ 2: Корутина с прогрессбаром ==========

struct Promise {
    int current_value = 0;

    auto get_return_object() {
        return std::coroutine_handle<Promise>::from_promise(*this);
    }

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}

    std::suspend_always yield_value(int value) {
        current_value = value;
        return {};
    }
};

struct Task {
    std::coroutine_handle<Promise> handle;

    Task(std::coroutine_handle<Promise> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }

    void resume() { handle.resume(); }
    bool done() const { return handle.done(); }
    int value() const { return handle.promise().current_value; }
};

namespace std {
    template<>
    struct coroutine_traits<Task, int> {
        using promise_type = Promise;
    };
}

Task simulate_work(int steps) {
    for (int i = 1; i <= steps; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        co_yield i; // Возвращаем прогресс
    }
    co_return;
}

void draw_progress_bar(int current, int total) {
    const int width = 50;
    float percent = (static_cast<float>(current) / total) * 100.0f;
    int filled = static_cast<int>((percent / 100.0f) * width);

    
    std::string name = "НАСТЯ"; 

    std::cout << "\r[";

    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            // Циклически используем символы имени
            std::cout << name[i % name.length()];
        }
        else {
            std::cout << " ";
        }
    }

    std::cout << "] " << std::fixed << std::setprecision(1)
        << percent << "% (" << current << "/" << total << ")";
    std::cout.flush();
}

void run_task2() {
    std::cout << "\n=== ПУНКТ 2 ===" << std::endl;
    std::cout << "Прогрессбар с именем" << std::endl;
    std::cout << "Имя в прогрессбаре: НАСТЯ" << std::endl;
    std::cout << "Запуск имитации работы..." << std::endl;
    std::cout << std::endl;

    const int total_steps = 100;
    Task task = simulate_work(total_steps);

    std::cout << "Выполнение: ";

    while (!task.done()) {
        task.resume();
        if (!task.done()) {
            draw_progress_bar(task.value(), total_steps);
        }
    }

    // Завершаем отрисовку
    draw_progress_bar(total_steps, total_steps);
    std::cout << "\n\nЗадача успешно завершена!" << std::endl;
}


int main() {
    // Устанавливаем локаль для поддержки русских символов
    setlocale(LC_ALL, "Russian");
    std::setlocale(LC_ALL, "Russian");

    std::cout << "ЛАБОРАТОРНАЯ РАБОТА №6: КОРУТИНЫ" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Студент: Настя" << std::endl;
    std::cout << "Число для демонстрации: 455" << std::endl;
    std::cout << "=================================" << std::endl;

    
    // Пункт 1
    run_task1();

    // Пауза перед пунктом 2
    std::cout << "\n\nПереход к пункту 2 через 2 секунды...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Пункт 2
    run_task2();

    std::cout << "\n=================================" << std::endl;
    std::cout << "Лабораторная работа №6 выполнена!" << std::endl;
    std::cout << "=================================" << std::endl;

    return 0;
}