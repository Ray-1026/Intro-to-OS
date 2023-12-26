/*
Student No.: 110550093
Student Name: 蔡師睿
Email: srt093.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <dirent.h>
#include <iostream>
#include <openssl/sha.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_map>
#define BUFFER_SIZE 1024 * 1024

int dedup_files = 0;
std::unordered_map<std::string, std::string> hash_table;

void computeSHA1andDedup(char *file)
{
    FILE *f = fopen(file, "r");
    int len = 0;
    SHA_CTX ctx;
    char buf[BUFFER_SIZE], hex_str[SHA_DIGEST_LENGTH * 2 + 1];
    unsigned char hash[SHA_DIGEST_LENGTH];

    // compute SHA1
    SHA1_Init(&ctx);
    while (len = fread(buf, 1, sizeof(buf), f))
        SHA1_Update(&ctx, buf, len);
    SHA1_Final(hash, &ctx);

    // unsigned char to hex string
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        sprintf(hex_str + i * 2, "%02x", hash[i]);
    std::string hash_str(hex_str);

    // dedup
    if (hash_table.find(hash_str) == hash_table.end())
        hash_table[hash_str] = file;
    else {
        dedup_files++;
        unlink(file);
        link(hash_table[hash_str].c_str(), file);
    }

    fclose(f);
}

void traversal(char *input_dir)
{
    DIR *dir = nullptr;
    struct dirent *dp = nullptr;
    struct stat st;

    if (stat(input_dir, &st) < 0 || !S_ISDIR(st.st_mode) || !(dir = opendir(input_dir))) {
        perror("Error");
        exit(1);
    }

    while ((dp = readdir(dir)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
            continue;

        char p[512];
        strncpy(p, input_dir, sizeof(p));
        strncat(p, "/", sizeof(p));
        strncat(p, dp->d_name, sizeof(p));
        stat(p, &st);

        if (S_ISDIR(st.st_mode))
            traversal(p);
        else
            computeSHA1andDedup(p);
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    traversal(argv[1]);

    gettimeofday(&end, NULL);
    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Naive Elapsed : %f seconds\nDedup files : %d\n", time, dedup_files);
}