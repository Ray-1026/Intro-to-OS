/*
Student No.: 110550093
Student Name: 蔡師睿
Email: srt093.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <sys/time.h>

using namespace std;

class ThreadPool {
  private:
    sem_t mutex, finish;
    int n, thread_num, arr[1000005], done[16] = {0};
    int table[16][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    pthread_t *threads;
    queue<int> work_queue;

    void bubblesort(int l, int r)
    {
        for (int i = l; i < r; i++) {
            for (int j = i + 1; j < r; j++) {
                if (arr[i] > arr[j])
                    swap(arr[i], arr[j]);
            }
        }
    }
    void merge(int begin, int mid, int end)
    {
        int left = begin, right = mid, index = 0, tmp[end - begin + 1];

        for (int i = begin; i < end; i++) {
            if (left < mid && right < end) {
                if (arr[left] < arr[right])
                    tmp[index++] = arr[left++];
                else
                    tmp[index++] = arr[right++];
            }
            else if (left < mid)
                tmp[index++] = arr[left++];
            else
                tmp[index++] = arr[right++];
        }

        for (int i = 0; i < index; i++)
            arr[begin + i] = tmp[i];
    }
    static void *merge_sort(void *this_pool)
    {
        ThreadPool *p = static_cast<ThreadPool *>(this_pool);

        for (;;) {
            sem_wait(&p->mutex);
            if (p->work_queue.empty()) {
                // no tasks
                sem_post(&p->mutex);
                sem_post(&p->finish);
                return NULL;
            }

            int job = p->work_queue.front();
            p->work_queue.pop();
            sem_post(&p->mutex);

            if (job > 7) {
                // bubble sort
                p->table[job][0] = (job - 8) * (p->n / 8);
                p->table[job][1] = (job != 15) ? (p->table[job][0] + (p->n / 8)) : (p->n);
                p->bubblesort(p->table[job][0], p->table[job][1]);
            }
            else {
                // merge
                p->table[job][0] = p->table[job * 2][0];
                p->table[job][1] = p->table[job * 2 + 1][1];
                p->merge(p->table[job][0], p->table[job * 2][1], p->table[job][1]);
            }

            sem_wait(&p->mutex);
            p->done[job] = 1;
            if (p->done[job + ((job & 1) ? -1 : 1)])
                p->work_queue.push(job >> 1);
            sem_post(&p->mutex);
        }
    }

  public:
    ThreadPool(int thread) : thread_num(thread) {}
    void init()
    {
        ifstream fin("input.txt");
        fin >> n;
        for (int i = 0; i < n; i++)
            fin >> arr[i];
        fin.close();

        sem_init(&mutex, 0, 1);
        sem_init(&finish, 0, 0);

        threads = new pthread_t[n];

        for (int job = 8; job < 16; job++)
            work_queue.push(job);
    }
    void runTasks()
    {
        for (int i = 0; i < thread_num; i++)
            pthread_create(&threads[i], NULL, merge_sort, this);

        for (int i = 0; i < thread_num; i++)
            sem_wait(&finish);
    }
    void write()
    {
        string filename = "output_" + to_string(thread_num) + ".txt";
        ofstream fout(filename);
        for (int i = 0; i < n; i++)
            fout << arr[i] << " ";
        fout.close();
    }
    void destroy()
    {
        delete[] threads;
        sem_destroy(&mutex);
        sem_destroy(&finish);
    }
};

int main()
{
    for (int thread = 1; thread < 9; thread++) {
        struct timeval start, stop;
        gettimeofday(&start, 0);

        ThreadPool *pool = new ThreadPool(thread);
        pool->init();
        pool->runTasks();
        pool->write();

        gettimeofday(&stop, 0);
        int sec = stop.tv_sec - start.tv_sec, usec = stop.tv_usec - start.tv_usec;
        cout << "worker thread #" << thread << ", elapsed " << (sec + (usec / 1000000.0)) * 1000 << " ms\n";

        pool->destroy();
    }
}