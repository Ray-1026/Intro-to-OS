/*
Student No.: 110550093
Student Name: 蔡師睿
Email: srt093.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main()
{
    int size;
    cout << "Input the matrix dimension:";
    cin >> size;
    cout << "\n";

    // shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(unsigned int) * size * size, IPC_CREAT | 0600);
    int *C = (int *)(shmat(shmid, NULL, 0));

    // different amount of processes
    for (int process = 1; process < 17; process++) {
        // initialize
        for (int i = 0; i < size * size; i++)
            C[i] = 0;

        int sub_row = size / process, remained = size % process;
        unsigned int checksum = 0;

        struct timeval start, end;
        gettimeofday(&start, 0);

        for (int p = 0; p < process; p++) {
            int allocated = sub_row + (p < remained), current = p * sub_row + (min(p, remained));
            pid_t pid = fork();
            if (pid == 0) {
                // child process
                for (int i = current; i < current + allocated; i++) {
                    for (int j = 0; j < size; j++) {
                        for (int k = 0; k < size; k++)
                            C[i * size + j] += (i * size + k) * (k * size + j);
                    }
                }
                exit(0);
            }
            else if (pid < 0) {
                fprintf(stderr, "Fork Failed");
                exit(-1);
            }
        }

        for (int i = 0; i < process; i++)
            wait(NULL);

        for (int i = 0; i < size * size; i++)
            checksum += C[i];

        gettimeofday(&end, 0);
        int sec = end.tv_sec - start.tv_sec, usec = end.tv_usec - start.tv_usec;
        cout << "Multiplying matrices using " << process << ((process == 1) ? (" process") : (" processes"))
             << "\nElapsed time: " << sec + (usec / 1000000.0) << " sec, Checksum: " << checksum << "\n";
    }

    shmdt(C);
    shmctl(shmid, IPC_RMID, NULL);
}