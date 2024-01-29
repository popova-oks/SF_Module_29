#include "../headers/FineGrainedQueue.h"
#include <queue>

template <typename T>
FineGrainedQueue<T>::FineGrainedQueue(int value)
    : head(new Node(value)), queue_mutex(new std::mutex) {}

template <typename T> FineGrainedQueue<T>::~FineGrainedQueue() {
    Node* cur = this->head;
    while(cur) {
        Node* next = cur->next;
        delete cur;
        cur = next;
    }
    delete queue_mutex;
}

template <typename T> void FineGrainedQueue<T>::insertIntoMiddle(int value, int pos) {
    Node *prev, *cur;
    queue_mutex->lock();
    prev = this->head;
    cur = this->head->next;

    prev->node_mutex->lock(); // залочили головной элемент
    if(cur) // проверили существует ли следующий элемент в списке
        cur->node_mutex->lock(); // залочили следующий элемент

    queue_mutex->unlock();
    int index = 1; // счетчик элементов

    // ищем нужный элемент
    while(cur && index < pos) {
        Node* old_prev = prev; // сохраняем указатель на предыдущий элемент
        prev = cur;        
        old_prev->node_mutex->unlock(); // разлочиваем предыдущий элемент

        cur = prev->next;
        if(cur) {
            cur->node_mutex->lock(); // залочили текущий элемент
        }
        index++;
    }

    if(cur == nullptr) {              // если pos больше длины списка
        prev->next = new Node(value); // создаем новый элемент
        prev->node_mutex->unlock();   // разлочиваем предыдущий элемент
    } else if(index == pos) { // если позиция текущего элемента найдена
        if(cur->next) { // если есть следующий элемент за текущим
            cur->next->node_mutex->lock(); // залочили следующий элемент за текущим
            prev->next = new Node(
                value, cur->next); // создаем новый элемент с указателем на следующий за текущим
            
            prev->node_mutex->unlock();                 // разлочиваем предыдущий элемент
            cur->next->node_mutex->unlock(); // разлочиваем следующий элемент за текущим
            cur->node_mutex->unlock(); // разлочиваем текущий элемент
            delete cur;                // удаляем текущий элемент
        } else {
            cur->node_mutex->unlock();
            prev->node_mutex->unlock();
        }
    }
}

template <typename T> void FineGrainedQueue<T>::sync_insertIntoMiddle(int value, int pos) {
    Node *prev, *cur;
    prev = this->head;
    cur = this->head->next;
    int index = 1; // счетчик элементов
    // ищем нужный элемент
    while(cur && index < pos) {
        Node* old_prev = prev; // сохраняем указатель на предыдущий элемент
        prev = cur;
        cur = cur->next;
        index++;
    }

    if(cur == nullptr) {              // если pos больше длины списка
        prev->next = new Node(value); // создаем новый элемент
    } else if(index == pos) { // если позиция текущего элемента найдена
        if(cur->next) { // если есть следующий элемент за текущим
            prev->next = new Node(
                value, cur->next); // создаем новый элемент с указателем на следующий за текущим
            delete cur;            // удаляем текущий элемент
        }
    }
}

template class FineGrainedQueue<int>;
