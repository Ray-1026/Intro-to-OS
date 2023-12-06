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
#include <list>
#include <sys/time.h>
#define HASH_SIZE 10007
#define hash_func(val) (val % HASH_SIZE)

using namespace std;

class PageReplacementAlgo {
  protected:
    int total_frame, cur_frame = 0, hit = 0, miss = 0;

  public:
    PageReplacementAlgo(int frame) : total_frame(frame) {}
    void printLog() { printf("%d\t%d\t\t%d\t\t%.10f\n", total_frame, hit, miss, (double)miss / (hit + miss)); }
};

//      __    ________  __
//     / /   / ____/ / / /
//    / /   / /_  / / / /
//   / /___/ __/ / /_/ /
//  /_____/_/    \____/
//

class LFU : public PageReplacementAlgo {
  private:
    struct FreqNode;
    struct Node {
        int val;
        Node *next, *prev;
        FreqNode *freq_node;
        Node(int val, Node *prev, Node *next) : val(val), prev(prev), next(next) {}
    };
    struct FreqNode {
        int freq;
        FreqNode *next, *prev;
        Node *nodes_head, *nodes_tail;
        FreqNode(int freq, FreqNode *prev, FreqNode *next) : freq(freq), prev(prev), next(next)
        {
            nodes_head = new Node(0, NULL, NULL);
            nodes_tail = new Node(0, nodes_head, NULL);
            nodes_head->next = nodes_tail;
        }
    };

    FreqNode *head, *tail;
    list<Node *> hash_table[HASH_SIZE];

    void insertNode(FreqNode *cur, Node *new_node)
    {
        new_node->prev = cur->nodes_head;
        new_node->next = cur->nodes_head->next;
        cur->nodes_head->next->prev = new_node;
        cur->nodes_head->next = new_node;
    }
    void removeEmptyFreqNode(FreqNode *cur)
    {
        if (cur != head && cur->nodes_head->next == cur->nodes_tail) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;
            delete cur;
        }
    }
    void lookup(int val)
    {
        for (auto &i : hash_table[hash_func(val)]) {
            if (i->val == val) {
                ++hit;

                // move the node to the next frequency node
                i->prev->next = i->next;
                i->next->prev = i->prev;

                FreqNode *next_freq = i->freq_node->next;
                if (next_freq->freq != i->freq_node->freq + 1) {
                    FreqNode *new_freq = new FreqNode(i->freq_node->freq + 1, i->freq_node, next_freq);
                    i->freq_node->next = new_freq;
                    next_freq->prev = new_freq;
                    next_freq = new_freq;
                }
                i->freq_node = next_freq;
                insertNode(next_freq, i);
                removeEmptyFreqNode(i->freq_node->prev);
                return;
            }
        }
        insert(val, hash_func(val));
    }
    void insert(int val, int hash_idx)
    {
        ++miss;
        if (cur_frame == total_frame)
            evict();
        else
            ++cur_frame;

        Node *new_node = new Node(val, NULL, NULL);
        FreqNode *least_freq = head->next;
        if (least_freq->freq != 1) {
            FreqNode *freq_1 = new FreqNode(1, head, least_freq);
            head->next = freq_1;
            least_freq->prev = freq_1;
            least_freq = freq_1;
        }
        new_node->freq_node = least_freq;
        insertNode(least_freq, new_node);

        hash_table[hash_idx].push_back(new_node);
    }
    void evict()
    {
        FreqNode *least_freq = head->next;
        Node *del_node = least_freq->nodes_tail->prev;
        del_node->prev->next = del_node->next;
        del_node->next->prev = del_node->prev;

        hash_table[hash_func(del_node->val)].remove(del_node);
        delete del_node;

        removeEmptyFreqNode(least_freq);
    }

  public:
    LFU(int frame) : PageReplacementAlgo(frame)
    {
        head = new FreqNode(0, NULL, NULL);
        tail = new FreqNode(1e8, head, NULL);
        head->next = tail;
    }
    void runSimulation(char *filename)
    {
        FILE *fin = fopen(filename, "r");
        char buf[10];
        while (fgets(buf, 10, fin))
            lookup(atoi(buf));
        fclose(fin);
    }
};

//      __    ____  __  __
//     / /   / __ \/ / / /
//    / /   / /_/ / / / /
//   / /___/ _, _/ /_/ /
//  /_____/_/ |_|\____/
//

class LRU : public PageReplacementAlgo {
  private:
    struct Node {
        int val;
        Node *next, *prev;
        Node(int val, Node *prev, Node *next) : val(val), prev(prev), next(next) {}
    };

    Node *head, *tail;
    list<Node *> hash_table[HASH_SIZE];

    void lookup(int val)
    {
        for (auto &i : hash_table[hash_func(val)]) {
            if (i->val == val) {
                ++hit;
                i->prev->next = i->next;
                i->next->prev = i->prev;
                i->prev = head;
                i->next = head->next;
                head->next->prev = i;
                head->next = i;
                return;
            }
        }
        insert(val, hash_func(val));
    }
    void insert(int val, int hash_idx)
    {
        ++miss;
        Node *new_node = new Node(val, head, head->next);
        head->next->prev = new_node;
        head->next = new_node;
        hash_table[hash_idx].push_back(new_node);

        // evict the least recently used page
        if (cur_frame == total_frame) {
            Node *del_node = tail->prev;
            tail->prev = del_node->prev;
            del_node->prev->next = tail;

            hash_table[hash_func(del_node->val)].remove(del_node);
            delete del_node;
        }
        else
            ++cur_frame;
    }

  public:
    LRU(int frame) : PageReplacementAlgo(frame)
    {
        head = new Node(0, NULL, NULL);
        tail = new Node(0, head, NULL);
        head->next = tail;
    }
    void runSimulation(char *filename)
    {
        FILE *fin = fopen(filename, "r");
        char buf[10];
        while (fgets(buf, 10, fin))
            lookup(atoi(buf));
        fclose(fin);
    }
};

int main(int argc, char *argv[])
{
    struct timeval start, end;
    printf("LFU policy:\nFrame\tHit\t\tMiss\t\tPage fault ratio\n");
    gettimeofday(&start, 0);

    LFU lfu_64(64), lfu_128(128), lfu_256(256), lfu_512(512);
    lfu_64.runSimulation(argv[1]);
    lfu_64.printLog();
    lfu_128.runSimulation(argv[1]);
    lfu_128.printLog();
    lfu_256.runSimulation(argv[1]);
    lfu_256.printLog();
    lfu_512.runSimulation(argv[1]);
    lfu_512.printLog();

    gettimeofday(&end, 0);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total elapsed time: %.4f sec\n\nLRU policy:\nFrame\tHit\t\tMiss\t\tPage fault ratio\n", elapsed_time);
    gettimeofday(&start, 0);

    LRU lru_64(64), lru_128(128), lru_256(256), lru_512(512);
    lru_64.runSimulation(argv[1]);
    lru_64.printLog();
    lru_128.runSimulation(argv[1]);
    lru_128.printLog();
    lru_256.runSimulation(argv[1]);
    lru_256.printLog();
    lru_512.runSimulation(argv[1]);
    lru_512.printLog();

    gettimeofday(&end, 0);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total elapsed time: %.4f sec\n", elapsed_time);
}
