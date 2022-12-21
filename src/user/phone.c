
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "../plutus.h"
#include "phone.h"


FILE *phdb = NULL;
FILE *pidb = NULL;

size_t root[CACHE_SIZ];


void print_root(void) {
    printf("-------------------------ROOT-------------------------\n");

    printf("     ");
    for (int i = 0; i < NODE_ABC; i++)
        printf("%d        ", i);

    printf("\n  0. ");

    for (int i = 0; i < CACHE_SIZ; i++) {
        if (i % 10 == 0 && i != 0)
            printf("\n%3d. ", i / 10);
        printf("%-8ld ", root[i]);
    }

    printf("\n------------------------------------------------------\n");
}

void print_node(Node node) {
    for (link_t i = 0; i < NODE_ABC; i++) {
        // if (i != 0) printf("|");
        printf(" %d:%-9ld", i, node[i]);
    }
    printf("\n");
}


// convert a phone number string
// to root index and links
Phone phone_convert(char *phone_number) {
    Phone phone;

    char cache_index[CACHE_LVL];
    strncpy(cache_index, phone_number, CACHE_LVL);

    phone.index = strtoul(cache_index, NULL, 10);

    for (link_t i = 0; i < LINKS_LEN; i++)
        phone.links[i] = phone_number[i + CACHE_LVL] - '0';

    return phone;
}


// add the node or update it
// return true if exists
bool phone_update(Phone *phone, user_id_t value) {
    size_t pos = root[phone->index];
    Node node;
    link_t link = 0;

    for (link_t i = 0; i < LINKS_LEN; i++) {
        link = phone->links[i];
        fseek(phdb, pos, SEEK_SET);
        fread(node, sizeof(Node), 1, phdb);

        // check of the current node is empty or not
        // if it is then we will write rest of the nodes
        if (node[link] == 0) {
            // read the file size (last byte position)
            size_t end_of_file = fsize(phdb);

            // go back to the current node for updating it
            fseek(phdb, pos, SEEK_SET);

            // set the next imaginary node position to the current one
            node[link] = end_of_file;

            // write the updated node
            fwrite(node, sizeof(Node), 1, phdb);

            // go to the end of the file for appending to it
            fseek(phdb, 0, SEEK_END);

            // empty the node
            memset(node, 0, sizeof(Node));

            // loop over ramaning links for writing them
            for (i++; i < LINKS_LEN; i++) {
                link = phone->links[i];

                // every node after this will be written at the next block
                // so we just imagine that this will be the next link position
                end_of_file += sizeof(Node);

                // check if where are at the end of our links
                // then set the last value to user_id
                // if we are not set the value to the next link
                if (i == LINKS_LEN - 1)
                    node[link] = value;
                else
                    node[link] = end_of_file;

                fwrite(node, sizeof(Node), 1, phdb);
                memset(node, 0, sizeof(Node));
            }

            return false;
        }

        if (i == LINKS_LEN - 1) {
            bool exists = node[link] != 0;

            node[link] = value;

            fseek(phdb, pos, SEEK_SET);
            fwrite(node, sizeof(Node), 1, phdb);

            return exists;
        }

        pos = node[link];
    }

    return false;
}


// search for a node
user_id_t phone_search(Phone *phone) {
    size_t pos = root[phone->index];
    Node node;

    for (link_t i = 0; i < LINKS_LEN; i++) {
        fseek(phdb, pos, SEEK_SET);
        fread(node, sizeof(Node), 1, phdb);
        pos = node[phone->links[i]];

        if (pos == 0) return 0;
        if (i == LINKS_LEN - 1) return pos;
    }

    return 0;
}


void phone_setup(void) {
    long pidb_size = fsize(pidb);
    long phdb_size = fsize(phdb);

    fseek(phdb, 0, SEEK_SET);
    fseek(pidb, 0, SEEK_SET);

    if (pidb_size < 0 || phdb_size < 0) {
        die("Error getting phone database size");
        return;
    }

    if (pidb_size == 0 && phdb_size == 0) {
        Node temp;
        memset(temp, 0, sizeof(Node));

        for (size_t i = 0; i < CACHE_SIZ; i++) {
            root[i] = i * sizeof(Node);
            fwrite(&root[i], sizeof(size_t), 1, pidb);
            fwrite(temp, sizeof(Node), 1, phdb);
        }

        return;
    }

    if (pidb_size == sizeof(size_t) * CACHE_SIZ &&
        (size_t)phdb_size >= sizeof(Node) * CACHE_SIZ) {
        fread(root, sizeof(size_t), CACHE_SIZ, pidb);
        return;
    }

    die("error loading the phone cache!");
}

