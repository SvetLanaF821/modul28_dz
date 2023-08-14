#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <fstream>

#define PART_SIZE 5000                      //размер части массива
#define ARR_SIZE 8'000'000                  //размер массива

//заполнение вектора
void vectorPadding(std::vector<int>& arr) {
    std::cout << "Заполнение вектора (размер " << ARR_SIZE << " элементов)\n";
    srand(time(nullptr));
    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = rand() % ARR_SIZE;
    }
    std::cout << "Bектор заполнен\n\n";
}

//вывод на печать всего вектора
void print(const std::vector<int>& arr)
{
    for (int num : arr)
        std::cout << num << " ";
    std::cout << std::endl;
}

//слияние
void merge(std::vector<int>& arr, int left, int mid, int right)
{
    std::vector<int> temp(right - left + 1);
    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (int p = 0; p < k; p++)
        arr[left + p] = temp[p];
}

//сортировка слиянием
void mergeSort(std::vector<int>& arr, int left, int right, bool flag)
{
    if (left < right) {
        if (right - left + 1 <= PART_SIZE) {
            std::sort(arr.begin() + left, arr.begin() + right + 1);
            return;
        }

        int mid = left + (right - left) / 2;

        if (flag && (right - left) > 1'000'000) {
            std::future<void> leftFuture = std::async(std::launch::async, mergeSort, std::ref(arr), left, mid, flag);
            std::future<void> rightFuture = std::async(std::launch::async, mergeSort, std::ref(arr), mid + 1, right, flag);

            leftFuture.get();
            rightFuture.get();
        }
        else {
            mergeSort(arr, left, mid, flag);
            mergeSort(arr, mid + 1, right, flag);
        }

        merge(arr, left, mid, right);
    }
}

//проверка сортировки
void sortCheck(std::vector<int>& arr) {
    bool arrSort = true;
    for (int i = 0; i < ARR_SIZE - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            std::cout << "Вектор не отсортирован\n\n";
            arrSort = false;
        }
    }
    if (arrSort)
        std::cout << "Вектор отсортирован\n\n";
}

int main()
{
    setlocale(LC_ALL, "");

    std::vector<int> arr(ARR_SIZE);

    vectorPadding(arr);                         //заполнение вектора

    bool flag = true;
    std::cout << "Сортировка в нескольких потоках\n";
    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(arr, 0, arr.size() - 1, flag);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;
    std::cout << "Затраченное время: " << elapsedTime.count() << " секунд\n";

    //print(arr);
    sortCheck(arr);                             //проверка правильности сортировки 

    vectorPadding(arr);                         //заполнение вектора

    flag = false;
    std::cout << "Сортировка в основном потоке\n";
    start = std::chrono::high_resolution_clock::now();
    mergeSort(arr, 0, arr.size() - 1, flag);
    finish = std::chrono::high_resolution_clock::now();
    elapsedTime = finish - start;
    std::cout << "Затраченное время: " << elapsedTime.count() << " секунд\n";

    //print(arr);
    sortCheck(arr);                             //проверка правильности сортировки 

    std::ofstream file("sortArray.txt");        //открытие файла для записи
    if (!file) {
        std::cout << "Ошибка. Файл не открыт." << std::endl;
        return 1;
    }

    std::cout << "Запись отсортированного вектора в файл\n";
    for (int i = 0; i < ARR_SIZE - 1; i++) {    
        file << arr[i] << " ";
    }
    std::cout << "Запись завершена\n";
    file.close();                               //закрытие файла

    return 0;
}